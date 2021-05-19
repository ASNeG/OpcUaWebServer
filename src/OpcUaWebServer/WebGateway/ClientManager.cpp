/*
   Copyright 2019-2020 Kai Huebl (kai@huebl-sgh.de)

   Lizenziert gemäß Apache Licence Version 2.0 (die „Lizenz“); Nutzung dieser
   Datei nur in Übereinstimmung mit der Lizenz erlaubt.
   Eine Kopie der Lizenz erhalten Sie auf http://www.apache.org/licenses/LICENSE-2.0.

   Sofern nicht gemäß geltendem Recht vorgeschrieben oder schriftlich vereinbart,
   erfolgt die Bereitstellung der im Rahmen der Lizenz verbreiteten Software OHNE
   GEWÄHR ODER VORBEHALTE – ganz gleich, ob ausdrücklich oder stillschweigend.

   Informationen über die jeweiligen Bedingungen für Genehmigungen und Einschränkungen
   im Rahmen der Lizenz finden Sie in der Lizenz.

   Autor: Kai Huebl (kai@huebl-sgh.de)
 */

#include <typeinfo>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/make_shared.hpp>
#include "OpcUaStackCore/Base/Log.h"
#include "OpcUaWebServer/WebGateway/ClientManager.h"
#include "OpcUaWebServer/WebGateway/ResponseHeader.h"
#include "OpcUaWebServer/WebGateway/SessionStatusNotify.h"
#include "OpcUaWebServer/WebGateway/SubscriptionStatusNotify.h"
#include "OpcUaWebServer/WebGateway/DataChangeNotify.h"
#include "OpcUaWebServer/WebGateway/EventNotify.h"

using namespace OpcUaStackCore;

namespace OpcUaWebServer
{

	ClientManager::ClientManager(void)
	: sendMessageCallback_()
	, disconnectChannelCallback_()
	, shutdownCallback_()
	, clientMap_()
	, channelIdSessionIdMap_()
	, ioThread_()
	, cryptoManager_()
	{
	}

	ClientManager::~ClientManager(void)
	{
	}

	bool
	ClientManager::startup(
		IOThread::SPtr& ioThread,
		MessageBus::SPtr& messageBus,
		CryptoManager::SPtr& cryptoManager
	)
	{
		ioThread_ = ioThread;
		messageBus_ = messageBus;
		cryptoManager_ = cryptoManager;
		return true;
	}

	bool
	ClientManager::shutdown(void)
	{
		Log(Debug, "stop opc ua client sessions")
			.parameter("NumberSessions", clientMap_.size());

		if (clientMap_.size() == 0) {
			return true;
		}

		// we must close all opc ua client sessions
		auto prom = std::promise<void>();
		auto future = prom.get_future();
		shutdownCallback_ = [this, &prom](void) {
			if (clientMap_.size() == 0) {
				prom.set_value();
			}
		};

		// shutdown all opc ua client sessions
		mutex_.lock();
		std::vector<Client::SPtr> clientVec;
		for (auto element : clientMap_) {
			clientVec.push_back(element.second);
		}
		mutex_.unlock();

		for (auto client : clientVec) {
			std::string sessionId = client->id();

			// logout complete handler
			auto logoutResponseCallback = [this, sessionId](OpcUaStatusCode statusCode, boost::property_tree::ptree& responseBody) mutable {
				mutex_.lock();
				auto it = clientMap_.find(sessionId);
				if (it != clientMap_.end()) {
					std::string sessionId = it->first;
					mutex_.unlock();
					std::thread thr([this, sessionId](){
						mutex_.lock();
						clientMap_.erase(sessionId);
						mutex_.unlock();
						if (shutdownCallback_) shutdownCallback_();
					});
					thr.detach();
				}
				else {
					mutex_.unlock();
					if (shutdownCallback_) shutdownCallback_();
				}
			};

			boost::property_tree::ptree requestBody;
			client->logout(requestBody, logoutResponseCallback);
		}

		future.wait();
		return true;
	}

	void
	ClientManager::sendMessageCallback(const SendMessageCallback& sendMessageCallback)
	{
		sendMessageCallback_ = sendMessageCallback;
	}

	void
	ClientManager::disconnectChannelCallback(const DisconnectChannelCallback& disconnectChannelCallback)
	{
		disconnectChannelCallback_ = disconnectChannelCallback;
	}

	void
	ClientManager::receiveMessage(WebSocketMessage::SPtr& webSocketMessage)
	{
		std::cout << "Command=" << webSocketMessage->command_ << std::endl;
		std::cout << "ChannelId=" << webSocketMessage->channelId_ << std::endl;
		std::cout << "ReceivedMessage=" << webSocketMessage->message_ << std::endl;

		if (shutdownCallback_) {
			// we want to shutdown and ignore all packages
			return;
		}

		// parse json message
		boost::property_tree::ptree pt;
		if (!Json::fromString(webSocketMessage->message_, pt, true)) {
			Log(Error, "json decode error")
				.parameter("ChannelId", webSocketMessage->channelId_);
			if (disconnectChannelCallback_) {
				disconnectChannelCallback_(webSocketMessage->channelId_);
			}
			return;
		}

		// get header from json message
		RequestHeader requestHeader;
		if (!requestHeader.jsonDecode(pt, "Header")) {
			Log(Error, "message header error");
			if (disconnectChannelCallback_) {
				disconnectChannelCallback_(webSocketMessage->channelId_);
			}
			return;
		}

		// get body document from json message
		boost::optional<boost::property_tree::ptree&> body = pt.get_child_optional("Body");
		if (!body) {
			Log(Error, "message do not contain message body");
			sendErrorResponse(webSocketMessage->channelId_, requestHeader, BadInvalidArgument);
			return;
		}

		auto messageType = requestHeader.messageType().toStdString();
		if (messageType == "CHANNELCLOSE_MESSAGE") {
			handleChannelClose(webSocketMessage->channelId_, requestHeader, *body);
			return;
		}

		else if (messageType == "GW_LoginRequest") {
			handleLogin(webSocketMessage->channelId_, requestHeader, *body);
			return;
		}

		else if (messageType == "GW_LogoutRequest") {
			handleLogout(webSocketMessage->channelId_, requestHeader, *body);
			return;
		}

		else {
			handleRequest(webSocketMessage->channelId_, requestHeader, *body);
		}
	}

	void
	ClientManager::handleChannelClose(
		uint32_t channelId,
		RequestHeader requestHeader,
		boost::property_tree::ptree& body
	)
	{
		Log(Debug, "WSG receive channel close request")
			.parameter("ChannelId", channelId);

		// remove all client sessions are assigned to the channel id
		auto result = channelIdSessionIdMap_.equal_range(channelId);
		for (auto it = result.first; it != result.second; it++) {

			// find client
			mutex_.lock();
			std::string sessionId = it->second;
			auto itc = clientMap_.find(sessionId);
			if (itc == clientMap_.end()) {
				continue;
			}
			auto client = itc->second;
			mutex_.unlock();

			// logout complete handler
			auto logoutResponseCallback = [this, channelId, sessionId](OpcUaStatusCode statusCode, boost::property_tree::ptree& responseBody) mutable {
				mutex_.lock();
				auto it = clientMap_.find(sessionId);
				if (it != clientMap_.end()) {
					std::string sessionId = it->first;
					mutex_.unlock();
					std::thread thr([this, sessionId](){
						mutex_.lock();
						clientMap_.erase(sessionId);
						mutex_.unlock();
						if (shutdownCallback_) shutdownCallback_();
					});
					thr.detach();
				}
				else {
					mutex_.unlock();
					if (shutdownCallback_) shutdownCallback_();
				}
			};

			boost::property_tree::ptree requestBody;
			client->logout(requestBody, logoutResponseCallback);
		}
		channelIdSessionIdMap_.erase(channelId);
	}

	void
	ClientManager::handleLogin(
		uint32_t channelId,
		RequestHeader requestHeader,
		boost::property_tree::ptree& requestBody
	)
	{
		std::string clientHandle = requestHeader.clientHandle();

		// create new opc ua client
		auto client = boost::make_shared<Client>();
		client->cryptoManager(cryptoManager_);
		client->ioThread(ioThread_);
		client->messageBus(messageBus_);
		auto sessionId = client->id();

		Log(Debug, "WSG receive login request")
			.parameter("ChannelId", channelId)
			.parameter("SessionId", sessionId)
			.parameter("ClientHandle", clientHandle);

		auto sessionStatusCallback = [this, channelId, clientHandle, sessionId](const std::string& sessionStatus) {
			NotifyHeader notifyHeader("GW_SessionStatusNotify", clientHandle, sessionId);

			SessionStatusNotify sessionStatusNotify;
			sessionStatusNotify.sessionStatus() = sessionStatus;

			boost::property_tree::ptree notifyBody;
			sessionStatusNotify.jsonEncode(notifyBody);
			sendNotify(channelId, notifyHeader, notifyBody);
		};

		// register subscription status callback
		auto subscriptionStatusCallback = [this, channelId, clientHandle, sessionId](uint32_t subscriptionId, const std::string& subscriptionStatus) {
			NotifyHeader notifyHeader("GW_SubscriptionStatusNotify", clientHandle, sessionId);

			SubscriptionStatusNotify subscriptionStatusNotify;
			subscriptionStatusNotify.subscriptionId() = subscriptionId;
			subscriptionStatusNotify.subscriptionStatus() = subscriptionStatus;

			boost::property_tree::ptree notifyBody;
			subscriptionStatusNotify.jsonEncode(notifyBody);
			//sendNotify(channelId, notifyHeader, notifyBody);
		};
		client->subscriptionStatusCallback(subscriptionStatusCallback);

		// register data change callback
		auto dataChangeCallback = [this, channelId, clientHandle, sessionId](uint32_t clientHandleData, const OpcUaDataValue& dataValue) {
			NotifyHeader notifyHeader("GW_DataChangeNotify", clientHandle, sessionId);

			DataChangeNotify dataChangeNotify;
			dataChangeNotify.clientHandle() = clientHandleData;
			dataChangeNotify.dataValue() = dataValue;

			boost::property_tree::ptree notifyBody;
			dataChangeNotify.jsonEncode(notifyBody);
			sendNotify(channelId, notifyHeader, notifyBody);
		};
		client->dataChangeCallback(dataChangeCallback);

		// register event change callback
		auto eventCallback = [this, channelId, clientHandle, sessionId](uint32_t clientHandleEvent, const OpcUaVariantArray& event) {
			NotifyHeader notifyHeader("GW_EventNotify", clientHandle, sessionId);

			EventNotify eventNotify;
			eventNotify.clientHandle() = clientHandleEvent;
			eventNotify.event() = event;

			boost::property_tree::ptree notifyBody;
			eventNotify.jsonEncode(notifyBody);
			sendNotify(channelId, notifyHeader, notifyBody);
		};
		client->eventCallback(eventCallback);

		// create new opc ua client session
		boost::property_tree::ptree responseBody;
		auto statusCode = client->login(requestBody, responseBody, sessionStatusCallback);
		if (statusCode != Success) {
			sendErrorResponse(channelId, requestHeader, statusCode);
			return;
		}

		// added client to manager map
		mutex_.lock();
		auto it = clientMap_.insert(std::make_pair(sessionId, client));
		if (!it.second) {
			mutex_.unlock();
			Log(Error, "client session id error")
				.parameter("Id", sessionId);
			sendErrorResponse(channelId, requestHeader, BadInternalError);
			return;
		}
		channelIdSessionIdMap_.insert(std::make_pair(channelId, sessionId));
		mutex_.unlock();

		// send login response
		sendResponse(channelId, requestHeader, responseBody);
	}

	void
	ClientManager::handleLogout(
		uint32_t channelId,
		RequestHeader requestHeader,
		boost::property_tree::ptree& requestBody
	)
	{
		Log(Debug, "WSG receive logout request")
			.parameter("ChannelId", channelId)
			.parameter("SessionId", requestHeader.sessionId())
			.parameter("ClientHandle", requestHeader.clientHandle());

		// find client
		mutex_.lock();
		auto it = clientMap_.find(requestHeader.sessionId());
		if (it == clientMap_.end()) {
			mutex_.unlock();
			sendErrorResponse(channelId, requestHeader, BadNotFound);
			return;
		}
		auto client = it->second;

		// remove element from client session map
		auto result = channelIdSessionIdMap_.equal_range(channelId);
		for (auto it = result.first; it != result.second; it++) {
			if (it->second == requestHeader.sessionId().toStdString()) {
				channelIdSessionIdMap_.erase(it);
				break;
			}
		}

		// remove client from client map
		clientMap_.erase(requestHeader.sessionId());

		mutex_.unlock();

		// logout
		auto logoutResponseCallback = [this, channelId, requestHeader, client](OpcUaStatusCode statusCode, boost::property_tree::ptree& responseBody) mutable {
			if (statusCode != Success) {
				sendErrorResponse(channelId, requestHeader, statusCode);
				return;
			}

			Log(Debug, "WSG remove session")
				.parameter("ChannelId", channelId)
				.parameter("SessionId", requestHeader.sessionId())
				.parameter("NumberSessions", clientMap_.size());

			sendResponse(channelId, requestHeader, responseBody);

			if (shutdownCallback_) shutdownCallback_();
		};

		client->logout(requestBody, logoutResponseCallback);
	}

	void
	ClientManager::handleRequest(
		uint32_t channelId,
		RequestHeader requestHeader,
		boost::property_tree::ptree& requestBody
	)
	{
		Log(Debug, "WSG receive data request")
			.parameter("ChannelId", channelId)
			.parameter("ClientHandle", requestHeader.clientHandle())
			.parameter("SessionId", requestHeader.sessionId())
			.parameter("Message", requestHeader.messageType());

		RequestInfo requestInfo;
		requestInfo.requestTimeout(requestHeader.requestTimeout());

		// find client
		mutex_.lock();
		auto it = clientMap_.find(requestHeader.sessionId());
		if (it == clientMap_.end()) {
			mutex_.unlock();
			sendErrorResponse(channelId, requestHeader, BadSessionClosed);
			return;
		}
		auto client = it->second;
		mutex_.unlock();

		// handle request
		auto messageResponseCallback = [this, channelId, requestHeader](OpcUaStatusCode statusCode, boost::property_tree::ptree& responseBody) mutable {
			if (statusCode != Success) {
				sendErrorResponse(channelId, requestHeader, statusCode);
			}
			else {
				sendResponse(channelId, requestHeader, responseBody);
			}
		};

		//
		// attribute service
		//
		auto messageType = requestHeader.messageType().toStdString();
		if (messageType == "GW_ReadRequest") {
			client->read(requestInfo, requestBody, messageResponseCallback);
		}
		else if (messageType == "GW_WriteRequest") {
			client->write(requestInfo, requestBody, messageResponseCallback);
		}
		else if (messageType == "GW_HistoryReadRequest") {
			client->historyRead(requestInfo, requestBody, messageResponseCallback);
		}

		//
		// view service
		//
		else if (messageType == "GW_BrowseRequest") {
			client->browse(requestInfo, requestBody, messageResponseCallback);
		}
		else if (messageType == "GW_BrowseNextRequest") {
			client->browseNext(requestInfo, requestBody, messageResponseCallback);
		}
		else if (messageType == "GW_TranslateBrowsePathsToNodeIdsRequest") {
			client->translateBrowsePathsToNodeIds(requestInfo, requestBody, messageResponseCallback);
		}

		//
		// method service
		//
		else if (messageType == "GW_CallRequest") {
			client->call(requestInfo, requestBody, messageResponseCallback);
		}

		//
		// subscription service
		//
		else if (messageType == "GW_CreateSubscriptionRequest") {
			client->createSubscription(requestInfo, requestBody, messageResponseCallback);
		}
		else if (messageType == "GW_DeleteSubscriptionsRequest") {
			client->deleteSubscriptions(requestInfo, requestBody, messageResponseCallback);
		}

		//
		// monitored item service
		//
		else if (messageType == "GW_CreateMonitoredItemsRequest") {
			client->createMonitoredItems(requestInfo, requestBody, messageResponseCallback);
		}
		else if (messageType == "GW_DeleteMonitoredItemsRequest") {
			client->deleteMonitoredItems(requestInfo, requestBody, messageResponseCallback);
		}

		//
		// error
		//
		else {
			sendErrorResponse(channelId, requestHeader, BadServiceUnsupported);
		}
	}

	void
	ClientManager::sendResponse(
		uint32_t channelId,
		RequestHeader& requestHeader,
		boost::property_tree::ptree& responseBody
	)
	{
		bool error = false;
		std::string errorMessage;
		boost::property_tree::ptree pt;

		// create header
		ResponseHeader responseHeader(requestHeader);
		responseHeader.statusCode() = Success;
		responseHeader.jsonEncode(pt, "Header");

		Log(Debug, "WSG send response")
			.parameter("ChannelId", channelId)
			.parameter("ClientHandle", responseHeader.clientHandle())
			.parameter("SessionId", responseHeader.sessionId())
			.parameter("Message", responseHeader.messageType());

		// create body
		pt.add_child("Body", responseBody);

		// create json message
		std::string msg;
		if (!Json::toString(pt, msg, true)) {
			Log(Error, "json parser error")
			    .parameter("ChannelId", channelId);
			return;
		}

		// create web socket message
		auto webSocketMessage = boost::make_shared<WebSocketMessage>();
		webSocketMessage->channelId_ = channelId;
		webSocketMessage->message_ = msg;

		if (sendMessageCallback_) {
			sendMessageCallback_(webSocketMessage);
		}
	}

	void
	ClientManager::sendNotify(
		uint32_t channelId,
		NotifyHeader& notifyHeader,
		boost::property_tree::ptree& notifyBody
	)
	{
		Log(Debug, "WSG send notify")
			.parameter("ChannelId", channelId)
			.parameter("SessionId", notifyHeader.sessionId())
			.parameter("Message", notifyHeader.messageType());

		bool error = false;
		std::string errorMessage;
		boost::property_tree::ptree pt;

		// create header
		notifyHeader.jsonEncode(pt, "Header");

		// create body
		pt.add_child("Body", notifyBody);

		// create json message
		std::string msg;
		if (!Json::toString(pt, msg, true)) {
			Log(Error, "json parser error")
			    .parameter("ChannelId", channelId);
			return;
		}

		// create web socket message
		auto webSocketMessage = boost::make_shared<WebSocketMessage>();
		webSocketMessage->channelId_ = channelId;
		webSocketMessage->message_ = msg;

		if (sendMessageCallback_) {
			sendMessageCallback_(webSocketMessage);
		}
	}

	void
	ClientManager::sendErrorResponse(
		uint32_t channelId,
		RequestHeader& requestHeader,
		OpcUaStatusCode statusCode
	)
	{
		bool error = false;
		std::string errorMessage;
		boost::property_tree::ptree pt;

		// create header
		ResponseHeader responseHeader(requestHeader);
		responseHeader.statusCode() = statusCode;
		responseHeader.jsonEncode(pt, "Header");

		Log(Debug, "WSG send error response")
			.parameter("ChannelId", channelId)
			.parameter("SessionId", responseHeader.sessionId())
			.parameter("Message", responseHeader.messageType())
			.parameter("StatusCode", OpcUaStatusCodeMap::shortString(statusCode));

		// create json message
		std::string msg;
		if (!Json::toString(pt, msg, true)) {
			Log(Error, "json parser error")
			    .parameter("ChannelId", channelId);
			return;
		}

		// create web socket message
		auto webSocketMessage = boost::make_shared<WebSocketMessage>();
		webSocketMessage->channelId_ = channelId;
		webSocketMessage->message_ = msg;

		if (sendMessageCallback_) {
			sendMessageCallback_(webSocketMessage);
		}
	}

}

