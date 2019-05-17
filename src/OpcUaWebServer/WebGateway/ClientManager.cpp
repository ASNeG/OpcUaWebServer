/*
   Copyright 2019 Kai Huebl (kai@huebl-sgh.de)

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
		CryptoManager::SPtr& cryptoManager
	)
	{
		ioThread_ = ioThread;
		cryptoManager_ = cryptoManager;
		return true;
	}

	bool
	ClientManager::shutdown(void)
	{
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
		for (auto element : clientMap_) {
			auto sessionId = element.first;
			auto client = element.second;

			// logout complete handler
			auto logoutResponseCallback = [this, sessionId](OpcUaStatusCode statusCode, boost::property_tree::ptree& responseBody) mutable {
				auto it = clientMap_.find(sessionId);
				clientMap_.erase(it);
				if (shutdownCallback_) shutdownCallback_();
			};

			boost::property_tree::ptree requestBody;
			client->logout(requestBody, logoutResponseCallback);
		}

		std::cout << "AA" << std::endl;
		future.wait();
		std::cout << "BB" << std::endl;
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
	ClientManager::receiveMessage(WebSocketMessage& webSocketMessage)
	{
		std::cout << "Command=" << webSocketMessage.command_ << std::endl;
		std::cout << "ChannelId=" << webSocketMessage.channelId_ << std::endl;
		std::cout << "ReceivedMessage=" << webSocketMessage.message_ << std::endl;

		boost::property_tree::ptree pt;
		std::stringstream ss;

		std::string errorMessage = "";
		bool error = false;

		// parse json message
		ss << webSocketMessage.message_;
		try {
			boost::property_tree::read_json(ss, pt);
		}
		catch (const boost::property_tree::json_parser_error& e)
		{
			errorMessage = std::string(e.what());
			error = true;
		}

		if (error) {
			Log(Error, "json parser error")
				.parameter("ChannelId", webSocketMessage.channelId_)
			    .parameter("Error", errorMessage);
			if (disconnectChannelCallback_) {
				disconnectChannelCallback_(webSocketMessage.channelId_);
			}
			return;
		}

		// get header from json message
		RequestHeader requestHeader;
		if (!requestHeader.jsonDecode(pt)) {
			Log(Error, "message header error");
			if (disconnectChannelCallback_) {
				disconnectChannelCallback_(webSocketMessage.channelId_);
			}
			return;
		}

		// get body document from json message
		boost::optional<boost::property_tree::ptree&> body = pt.get_child_optional("Body");
		if (!body) {
			Log(Error, "message do not contain message body");
			sendErrorResponse(webSocketMessage.channelId_, requestHeader, BadInvalidArgument);
			return;
		}

		if (requestHeader.messageType() == "CHANNELCLOSE_MESSAGE") {
			handleChannelClose(webSocketMessage.channelId_, requestHeader, *body);
			return;
		}

		else if (requestHeader.messageType() == "GW_LoginRequest") {
			handleLogin(webSocketMessage.channelId_, requestHeader, *body);
			return;
		}

		else if (requestHeader.messageType() == "GW_LogoutRequest") {
			handleLogout(webSocketMessage.channelId_, requestHeader, *body);
			return;
		}

		else {
			handleRequest(webSocketMessage.channelId_, requestHeader, *body);
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
			std::string sessionId = it->second;
			auto itc = clientMap_.find(sessionId);
			if (itc == clientMap_.end()) {
				continue;
			}
			auto client = itc->second;

			// logout complete handler
			auto logoutResponseCallback = [this, channelId, sessionId](OpcUaStatusCode statusCode, boost::property_tree::ptree& responseBody) mutable {
				Log(Debug, "WSG remove client session")
					.parameter("ChannelId", channelId)
					.parameter("SessionId", sessionId);
				auto it = clientMap_.find(sessionId);
				clientMap_.erase(it);
				if (shutdownCallback_) shutdownCallback_();
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
		client->ioThread(ioThread_);
		client->cryptoManager(cryptoManager_);
		auto sessionId = client->id();

		Log(Debug, "WSG receive login request")
			.parameter("ChannelId", channelId)
			.parameter("SessionId", sessionId);

		auto sessionStatusCallback = [this, channelId, clientHandle, sessionId](const std::string& sessionStatus) {
			NotifyHeader notifyHeader("GW_SessionStatusNotify", clientHandle, sessionId);

			SessionStatusNotify sessionStatusNotify;
			sessionStatusNotify.sessionStatus() = sessionStatus;

			boost::property_tree::ptree notifyBody;
			sessionStatusNotify.jsonEncode(notifyBody);
			sendNotify(channelId, notifyHeader, notifyBody);
		};

		// create new opc ua client session
		boost::property_tree::ptree responseBody;
		auto statusCode = client->login(requestBody, responseBody, sessionStatusCallback);
		if (statusCode != Success) {
			sendErrorResponse(channelId, requestHeader, statusCode);
			return;
		}

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

		// added client to manager map
		auto it = clientMap_.insert(std::make_pair(sessionId, client));
		if (!it.second) {
			Log(Error, "client session id error")
				.parameter("Id", sessionId);
			sendErrorResponse(channelId, requestHeader, BadInternalError);
			return;
		}
		channelIdSessionIdMap_.insert(std::make_pair(channelId, sessionId));

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
			.parameter("SessionId", requestHeader.sessionId());

		// find client
		auto it = clientMap_.find(requestHeader.sessionId());
		if (it == clientMap_.end()) {
			sendErrorResponse(channelId, requestHeader, BadNotFound);
			return;
		}
		auto client = it->second;

		// logout
		auto logoutResponseCallback = [this, channelId, requestHeader](OpcUaStatusCode statusCode, boost::property_tree::ptree& responseBody) mutable {
			if (statusCode != Success) {
				sendErrorResponse(channelId, requestHeader, statusCode);
			}
			else {
				Log(Debug, "WSG remove session")
					.parameter("ChannelId", channelId)
					.parameter("SessionId", requestHeader.sessionId());

				sendResponse(channelId, requestHeader, responseBody);

				auto it = clientMap_.find(requestHeader.sessionId());
				if (it != clientMap_.end()) {
					clientMap_.erase(it);
				}

				if (shutdownCallback_) shutdownCallback_();
			}
		};

		// remove element from client sesssion map
		auto result = channelIdSessionIdMap_.equal_range(channelId);
		for (auto it = result.first; it != result.second; it++) {
			if (it->second == requestHeader.sessionId()) {
				channelIdSessionIdMap_.erase(it);
			}
		}

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
			.parameter("SessionId", requestHeader.sessionId())
			.parameter("Message", requestHeader.messageType());

		// find client
		auto it = clientMap_.find(requestHeader.sessionId());
		if (it == clientMap_.end()) {
			sendErrorResponse(channelId, requestHeader, BadSessionClosed);
			return;
		}
		auto client = it->second;

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
		if (requestHeader.messageType() == "GW_ReadRequest") {
			client->read(requestBody, messageResponseCallback);
		}
		else if (requestHeader.messageType() == "GW_WriteRequest") {
			client->write(requestBody, messageResponseCallback);
		}
		else if (requestHeader.messageType() == "GW_HistoryReadRequest") {
			client->historyRead(requestBody, messageResponseCallback);
		}

		//
		// method service
		//
		else if (requestHeader.messageType() == "GW_CallRequest") {
			client->call(requestBody, messageResponseCallback);
		}

		//
		// subscription service
		//
		else if (requestHeader.messageType() == "GW_CreateSubscriptionRequest") {
			client->createSubscription(requestBody, messageResponseCallback);
		}
		else if (requestHeader.messageType() == "GW_DeleteSubscriptionsRequest") {
			client->deleteSubscriptions(requestBody, messageResponseCallback);
		}

		//
		// monitored item service
		//
		else if (requestHeader.messageType() == "GW_CreateMonitoredItemsRequest") {
			client->createMonitoredItems(requestBody, messageResponseCallback);
		}
		else if (requestHeader.messageType() == "GW_DeleteMonitoredItemsRequest") {
			client->deleteMonitoredItems(requestBody, messageResponseCallback);
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
		responseHeader.jsonEncode(pt);

		Log(Debug, "WSG send response")
			.parameter("ChannelId", channelId)
			.parameter("SessionId", responseHeader.sessionId())
			.parameter("Message", responseHeader.messageType());

		// create body
		pt.add_child("Body", responseBody);

		// create json message
		std::stringstream msg;
		try {
			boost::property_tree::write_json(msg, pt);
		}
		catch (const boost::property_tree::json_parser_error& e)
		{
			errorMessage = std::string(e.what());
			error = true;
		}

		if (error) {
			Log(Error, "json parser error")
			    .parameter("ChannelId", channelId)
			    .parameter("Error", errorMessage);
			return;
		}

		// create web socket message
		WebSocketMessage webSocketMessage;
		webSocketMessage.channelId_ = channelId;
		webSocketMessage.message_ = msg.str();

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
		notifyHeader.jsonEncode(pt);

		// create body
		pt.add_child("Body", notifyBody);

		// create json message
		std::stringstream msg;
		try {
			boost::property_tree::write_json(msg, pt);
		}
		catch (const boost::property_tree::json_parser_error& e)
		{
			errorMessage = std::string(e.what());
			error = true;
		}

		if (error) {
			Log(Error, "json parser error")
			    .parameter("ChannelId", channelId)
			    .parameter("Error", errorMessage);
			return;
		}

		// create web socket message
		WebSocketMessage webSocketMessage;
		webSocketMessage.channelId_ = channelId;
		webSocketMessage.message_ = msg.str();

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
		responseHeader.jsonEncode(pt);

		Log(Debug, "WSG send error response")
			.parameter("ChannelId", channelId)
			.parameter("SessionId", responseHeader.sessionId())
			.parameter("Message", responseHeader.messageType())
			.parameter("StatusCode", OpcUaStatusCodeMap::shortString(statusCode));

		// create json message
		std::stringstream msg;
		try {
			boost::property_tree::write_json(msg, pt);
		}
		catch (const boost::property_tree::json_parser_error& e)
		{
			errorMessage = std::string(e.what());
			error = true;
		}

		if (error) {
			Log(Error, "json parser error")
			    .parameter("ChannelId", channelId)
			    .parameter("Error", errorMessage);
			return;
		}

		// create web socket message
		WebSocketMessage webSocketMessage;
		webSocketMessage.channelId_ = channelId;
		webSocketMessage.message_ = msg.str();

		if (sendMessageCallback_) {
			sendMessageCallback_(webSocketMessage);
		}
	}

}

