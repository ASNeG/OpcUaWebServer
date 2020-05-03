/*
   Copyright 2015-2020 Kai Huebl (kai@huebl-sgh.de)

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

#include "OpcUaStackCore/Base/Log.h"
#include "OpcUaWebServer/OpcUaClient/OpcUaClientManager.h"
#include "OpcUaWebServer/OpcUaClient/ReadRequestData.h"
#include "OpcUaWebServer/OpcUaClient/ReadResponseData.h"
#include "OpcUaWebServer/OpcUaClient/WriteRequestData.h"
#include "OpcUaWebServer/OpcUaClient/WriteResponseData.h"
#include "OpcUaWebServer/OpcUaClient/ValueListResponseData.h"
#include "OpcUaWebServer/OpcUaClient/ValueInfoRequestData.h"
#include "OpcUaWebServer/OpcUaClient/ValueInfoResponseData.h"
#include "OpcUaWebServer/OpcUaClient/MonitorStartResponseData.h"
#include "OpcUaWebServer/OpcUaClient/MonitorStopResponseData.h"
#include "OpcUaWebServer/Messages/Message.h"

using namespace OpcUaStackCore;

namespace OpcUaWebServer
{

	OpcUaClientManager::OpcUaClientManager(void)
	: enable_(true)
	, config_(nullptr)
	, opcUaClientManagerIf_(nullptr)
	, opcUaClientConfigMap_()
	, opcUaClientMap_()
	, valueInfoEntryMap_()
	, cryptoManager_()
	{
	}

	OpcUaClientManager::~OpcUaClientManager(void)
	{
	}

	bool
	OpcUaClientManager::startup(
		Config* config,
		OpcUaClientManagerIf* opcUaClientManagerIf,
		IOThread::SPtr ioThread,
		MessageBus::SPtr& messageBus,
		CryptoManager::SPtr& cryptoManager
	)
	{
		config_ = config;
		opcUaClientManagerIf_ = opcUaClientManagerIf;
		ioThread_ = ioThread;
		messageBus_ = messageBus;
		cryptoManager_ = cryptoManager;

		if (!readClientConfig()) return false;

		if (!enable_) {
			return true;
		}

		if (!startupClient()) return false;

		return true;
	}

	bool
	OpcUaClientManager::shutdown(void)
	{
		if (!enable_) {
			return true;
		}

		return true;
	}

	void
	OpcUaClientManager::receiveMessage(Message::SPtr& requestMessage)
	{
		switch (requestMessage->messageType())
		{
			case Message::MT_ReadRequest:
			{
				receiveReadMessage(requestMessage);
				break;
			}
			case Message::MT_WriteRequest:
			{
				receiveWriteMessage(requestMessage);
				break;
			}
			case Message::MT_ValueListRequest:
			{
				receiveValueListMessage(requestMessage);
				break;
			}
			case Message::MT_ValueInfoRequest:
			{
				receiveValueInfoMessage(requestMessage);
				break;
			}
			case Message::MT_MonitorStartRequest:
			{
				receiveMonitorStartMessage(requestMessage);
				break;
			}
			case Message::MT_MonitorStopRequest:
			{
				receiveMonitorStopMessage(requestMessage);
				break;
			}
			case Message::MT_HistorcialReadRequest:
			{
				receiveHistoricalReadMessage(requestMessage);
				break;
			}
			case Message::MT_ChannelCloseMessage:
			{
				receiveChannelCloseMessage(requestMessage);
				break;
			}
			default:
			{
				Log(Error, "receive message with invalid message type")
					.parameter("ChannelId", requestMessage->channelId())
					.parameter("MessageType", requestMessage->messageType());

				// send error message to channel

				sendErrorMessage(requestMessage, Message::MT_Error, BadRequestHeaderInvalid);
			}
		}
	}

	void
	OpcUaClientManager::receiveReadMessage(Message::SPtr& requestMessage)
	{
		Log(Debug, "receive read message")
			.parameter("ChannelId", requestMessage->channelId())
			.parameter("ClientHandle", requestMessage->clientHandle());

		ReadRequestData readRequestData;
		if (!readRequestData.decode(requestMessage->body())) {
			sendErrorMessage(requestMessage, Message::MT_ReadResponse, OpcUaStatusCode::BadAttributeIdInvalid);
			return;
		}

		// find responsible opc ua client
		auto it = valueInfoEntryMap_.find(readRequestData.variable());
		if (it == valueInfoEntryMap_.end()) {
			// no responsible opc ua client found

			ReadResponseData readResponseData;
			readResponseData.statusCode(OpcUaStatusCodeMap::shortString(BadNodeIdUnknown));

			Message::SPtr responseMessage = boost::make_shared<Message>(Message::MT_ReadResponse);
			responseMessage->channelId(requestMessage->channelId());
			responseMessage->clientHandle(requestMessage->clientHandle());

			boost::property_tree::ptree pt;
			if (!readResponseData.jsonEncode(pt)) {
				responseMessage->statusCode(OpcUaStatusCodeMap::shortString(BadInternalError));
			}
			else {
				responseMessage->body(pt);
			}
			opcUaClientManagerIf_->clientManagerMessage(responseMessage);
			return;
		}
		ValueInfoEntry::SPtr valueInfoEntry = it->second;

		auto opcUaClient = boost::static_pointer_cast<OpcUaClient>(valueInfoEntry->object_);
		opcUaClient->readRequest(requestMessage, readRequestData, valueInfoEntry);
	}

	void
	OpcUaClientManager::receiveWriteMessage(Message::SPtr& requestMessage)
	{
		Log(Debug, "receive write message")
			.parameter("ChannelId", requestMessage->channelId())
			.parameter("ClientHandle", requestMessage->clientHandle());

		// get variable name from write request
		WriteRequestData writeRequestData;
		if (!writeRequestData.decodeVariable(requestMessage->body())) {
			sendErrorMessage(requestMessage, Message::MT_WriteResponse, OpcUaStatusCode::BadAttributeIdInvalid);
			return;
		}

		// find responsible opc ua client
		auto it = valueInfoEntryMap_.find(writeRequestData.variable());
		if (it == valueInfoEntryMap_.end()) {
			// no responsible opc ua client found
			WriteResponseData writeResponseData;
			writeResponseData.statusCode(OpcUaStatusCodeMap::shortString(BadNodeIdUnknown));

			Message::SPtr responseMessage = boost::make_shared<Message>(Message::MT_WriteResponse);
			responseMessage->channelId(requestMessage->channelId());
			responseMessage->clientHandle(requestMessage->clientHandle());

			boost::property_tree::ptree pt;
			if (!writeResponseData.jsonEncode(pt)) {
				responseMessage->statusCode(OpcUaStatusCodeMap::shortString(BadInternalError));
			}
			else {
				responseMessage->body(pt);
			}
			opcUaClientManagerIf_->clientManagerMessage(responseMessage);
			return;
		}
		ValueInfoEntry::SPtr valueInfoEntry = it->second;

		writeRequestData.valueInfoEntry(valueInfoEntry);
		if (!writeRequestData.jsonDecode(requestMessage->body())) {
			sendErrorMessage(requestMessage, Message::MT_WriteResponse, OpcUaStatusCode::BadAttributeIdInvalid);
			return;
		}

		auto opcUaClient = boost::static_pointer_cast<OpcUaClient>(valueInfoEntry->object_);
		opcUaClient->writeRequest(requestMessage, writeRequestData, valueInfoEntry);
	}

	void
	OpcUaClientManager::receiveValueListMessage(Message::SPtr& requestMessage)
	{
		Log(Debug, "receive value list message")
			.parameter("ChannelId", requestMessage->channelId())
			.parameter("ClientHandle", requestMessage->clientHandle());

		Message::SPtr responseMessage = boost::make_shared<Message>(Message::MT_ValueListResponse);
		responseMessage->channelId(requestMessage->channelId());
		responseMessage->clientHandle(requestMessage->clientHandle());

		ValueListResponseData valueListResponseData;
		ValueInfoEntry::Map::iterator it;
		for (it = valueInfoEntryMap_.begin(); it != valueInfoEntryMap_.end(); it++) {
			valueListResponseData.valueVec().push_back(it->first);
		}

		// create response body
		boost::property_tree::ptree pt;
		if (!valueListResponseData.jsonEncode(pt)) {
			sendErrorMessage(requestMessage, Message::MT_ValueListResponse, OpcUaStatusCode::BadInternalError);
			return;
		}

		responseMessage->body(pt);
		opcUaClientManagerIf_->clientManagerMessage(responseMessage);
	}

	void
	OpcUaClientManager::receiveValueInfoMessage(Message::SPtr& requestMessage)
	{
		Log(Debug, "receive value info message")
			.parameter("ChannelId", requestMessage->channelId())
			.parameter("ClientHandle", requestMessage->clientHandle());

		Message::SPtr responseMessage = boost::make_shared<Message>(Message::MT_ValueInfoResponse);
		responseMessage->channelId(requestMessage->channelId());
		responseMessage->clientHandle(requestMessage->clientHandle());

		ValueInfoRequestData valueInfoRequestData;
		if (!valueInfoRequestData.jsonDecode(requestMessage->body())) {
			sendErrorMessage(requestMessage, Message::MT_ValueInfoResponse, OpcUaStatusCode::BadAttributeIdInvalid);
			return;
		}

		ValueInfoResponseData valueInfoResponseData;
		for (
			auto it1 = valueInfoRequestData.variableVec().begin();
			it1 != valueInfoRequestData.variableVec().end();
			it1++) {
			ValueInfoData valueInfoData;
			valueInfoData.variableName(*it1);

			ValueInfoEntry::Map::iterator it2;
			it2 = valueInfoEntryMap_.find(*it1);

			if (it2 == valueInfoEntryMap_.end()) {
				valueInfoData.statusCode(OpcUaStatusCodeMap::shortString(BadNodeIdUnknown));
			}
			else {
				valueInfoData.variableType(OpcUaBuildInTypeMap::buildInType2String(it2->second->type_));
				valueInfoData.isArray(it2->second->array_);
				valueInfoData.metaData(it2->second->metaData_);
			}

			valueInfoResponseData.valueInfoDataVec().push_back(valueInfoData);
		}

		// create response body
		boost::property_tree::ptree pt;
		if (!valueInfoResponseData.encode(pt)) {
			responseMessage->statusCode(OpcUaStatusCodeMap::shortString(BadInternalError));
			opcUaClientManagerIf_->clientManagerMessage(responseMessage);
			return;
		}

		responseMessage->body(pt);
		opcUaClientManagerIf_->clientManagerMessage(responseMessage);
	}

	void
	OpcUaClientManager::receiveMonitorStartMessage(Message::SPtr& requestMessage)
	{
		Log(Debug, "receive monitor start message")
			.parameter("ChannelId", requestMessage->channelId())
			.parameter("ClientHandle", requestMessage->clientHandle());

		MonitorStartRequestData monitorStartRequestData;
		if (!monitorStartRequestData.decode(requestMessage->body())) {
			sendErrorMessage(requestMessage, Message::MT_MonitorStartResponse, OpcUaStatusCode::BadAttributeIdInvalid);
			return;
		}

		// find responsible opc ua client
		auto it = valueInfoEntryMap_.find(monitorStartRequestData.variable());
		if (it == valueInfoEntryMap_.end()) {
			// no responsible opc ua client found
			MonitorStartResponseData monitorStartResponseData;
			monitorStartResponseData.statusCode(OpcUaStatusCodeMap::shortString(BadNodeIdUnknown));

			Message::SPtr responseMessage = boost::make_shared<Message>(Message::MT_MonitorStartResponse);
			responseMessage->channelId(requestMessage->channelId());
			responseMessage->clientHandle(requestMessage->clientHandle());

			boost::property_tree::ptree pt;
			if (!monitorStartResponseData.jsonEncode(pt)) {
				responseMessage->statusCode(OpcUaStatusCodeMap::shortString(BadInternalError));
			}
			else {
				responseMessage->body(pt);
			}
			opcUaClientManagerIf_->clientManagerMessage(responseMessage);
			return;
		}
		ValueInfoEntry::SPtr valueInfoEntry = it->second;

		auto opcUaClient = boost::static_pointer_cast<OpcUaClient>(valueInfoEntry->object_);
		opcUaClient->monitorStartRequest(requestMessage, monitorStartRequestData, valueInfoEntry);
	}

	void
	OpcUaClientManager::receiveMonitorStopMessage(Message::SPtr& requestMessage)
	{
		Log(Debug, "receive monitor stop message")
			.parameter("ChannelId", requestMessage->channelId())
			.parameter("ClientHandle", requestMessage->clientHandle());

		MonitorStopRequestData monitorStopRequestData;
		if (!monitorStopRequestData.decode(requestMessage->body())) {
			sendErrorMessage(requestMessage, Message::MT_MonitorStopResponse, OpcUaStatusCode::BadAttributeIdInvalid);
			return;
		}

		// find responsible opc ua client
		auto it = valueInfoEntryMap_.find(monitorStopRequestData.variable());
		if (it == valueInfoEntryMap_.end()) {
			// no responsible opc ua client found
			MonitorStopResponseData monitorStopResponseData;
			monitorStopResponseData.statusCode(OpcUaStatusCodeMap::shortString(BadNodeIdUnknown));

			Message::SPtr responseMessage = boost::make_shared<Message>(Message::MT_MonitorStopResponse);
			responseMessage->channelId(requestMessage->channelId());
			responseMessage->clientHandle(requestMessage->clientHandle());

			boost::property_tree::ptree pt;
			if (!monitorStopResponseData.jsonEncode(pt)) {
				responseMessage->statusCode(OpcUaStatusCodeMap::shortString(BadInternalError));
			}
			else {
				responseMessage->body(pt);
			}
			opcUaClientManagerIf_->clientManagerMessage(responseMessage);
			return;
		}
		ValueInfoEntry::SPtr valueInfoEntry = it->second;

		auto opcUaClient = boost::static_pointer_cast<OpcUaClient>(valueInfoEntry->object_);
		opcUaClient->monitorStopRequest(requestMessage, monitorStopRequestData, valueInfoEntry);
	}

	void
	OpcUaClientManager::receiveHistoricalReadMessage(Message::SPtr& requestMessage)
	{
		Log(Debug, "receive historical read message")
			.parameter("ChannelId", requestMessage->channelId())
			.parameter("ClientHandle", requestMessage->clientHandle());

		HistoricalReadRequestData historicalReadRequestData;
		if (!historicalReadRequestData.decode(requestMessage->body())) {
			sendErrorMessage(requestMessage, Message::MT_HistoricalReadResponse, OpcUaStatusCode::BadAttributeIdInvalid);
			return;
		}

		// find responsible opc ua client
		auto it = valueInfoEntryMap_.find(historicalReadRequestData.variable());
		if (it == valueInfoEntryMap_.end()) {
			// no responsible opc ua client found
			HistoricalReadResponseData historicalReadResponseData;
			historicalReadResponseData.statusCode(OpcUaStatusCodeMap::shortString(BadNodeIdUnknown));

			Message::SPtr responseMessage = boost::make_shared<Message>(Message::MT_HistoricalReadResponse);
			responseMessage->channelId(requestMessage->channelId());
			responseMessage->clientHandle(requestMessage->clientHandle());

			boost::property_tree::ptree pt;
			if (!historicalReadResponseData.jsonEncode(pt)) {
				responseMessage->statusCode(OpcUaStatusCodeMap::shortString(BadInternalError));
			}
			else {
				responseMessage->body(pt);
			}
			opcUaClientManagerIf_->clientManagerMessage(responseMessage);
			return;
		}
		ValueInfoEntry::SPtr valueInfoEntry = it->second;

		auto opcUaClient = boost::static_pointer_cast<OpcUaClient>(valueInfoEntry->object_);
		opcUaClient->historicalReadRequest(requestMessage, historicalReadRequestData, valueInfoEntry);
	}

	void
	OpcUaClientManager::receiveChannelCloseMessage(Message::SPtr& requestMessage)
	{
		for (auto it = opcUaClientMap_.begin(); it != opcUaClientMap_.end(); it++) {
			OpcUaClient::SPtr opcUaClient = it->second;
			opcUaClient->closeChannelMessage(requestMessage);
		}
	}

	void
	OpcUaClientManager::clientMessage(Message::SPtr& message)
	{
		opcUaClientManagerIf_->clientManagerMessage(message);
	}

	bool
	OpcUaClientManager::readClientConfig(void)
	{
		// check diable flag
		if (
			!config_->exist("OpcUaWebServerModel.OpcUaClient") ||
			config_->exist("OpcUaWebServerModel.OpcUaClient.<xmlattr>.Disable")
		) {
			enable_ = false;
			return true;
		}

		// read client files from configuration
		std::vector<std::string> clientConfigFileVec;
		config_->getValues("OpcUaWebServerModel.OpcUaClient.ClientConfigFile", clientConfigFileVec);
		if (clientConfigFileVec.size() == 0) {
			Log(Error, "no client configuration available in client configuration")
				.parameter("ConfigFileName", config_->configFileName())
				.parameter("Parameter", "OpcUaWebServerModel.OpcUaClient.ClientConfigFile");
			return false;
		}

		// parse client configuration
		for (auto it = clientConfigFileVec.begin(); it != clientConfigFileVec.end(); it++) {
			Log(Debug, "parse client configuration")
				.parameter("ConfigFileName", *it);

			OpcUaClientConfig::SPtr opcUaClientConfig = boost::make_shared<OpcUaClientConfig>();

			if (!opcUaClientConfig->decode(*it)) {
				Log(Error, "parse client configuration error")
					.parameter("ConfigFileName", *it);
				return false;
			}
			opcUaClientConfigMap_.insert(
				std::make_pair(opcUaClientConfig->name_, opcUaClientConfig)
			);
		}

		return true;
	}

	bool
	OpcUaClientManager::startupClient(void)
	{
		// start clients
		for (auto it1 = opcUaClientConfigMap_.begin(); it1 != opcUaClientConfigMap_.end(); it1++) {
			OpcUaClient::SPtr opcUaClient = boost::make_shared<OpcUaClient>();

			Log(Info, "client startup")
			    .parameter("ConfigFileName", it1->first);

			opcUaClient->config(config_);
			opcUaClient->opcUaClientConfig(it1->second);
			opcUaClient->opcUaClientIf(this);

			if (!opcUaClient->startup(opcUaClient, ioThread_, messageBus_, cryptoManager_)) return false;

			opcUaClientMap_.insert(std::make_pair(it1->first, opcUaClient));

			// get values names from opc ua client and create value access map entries
			for (
				auto it2 = opcUaClient->valueInfoEntryMap().begin();
				it2 != opcUaClient->valueInfoEntryMap().end();
				it2++
			) {
				if (valueInfoEntryMap_.find(it2->first) != valueInfoEntryMap_.end()) {
					Log(Error, "found duplicate value names in different client configurations")
						.parameter("ValueName", it2->first);
					return false;
				}
				valueInfoEntryMap_.insert(std::make_pair(it2->first, it2->second));
			}
		}

		return true;
	}


	void
	OpcUaClientManager::sendErrorMessage(Message::SPtr &requestMessage, Message messageType, OpcUaStatusCode statusCode) const
	{
		Message::SPtr responseMessage = boost::make_shared<Message>(messageType);
		responseMessage->channelId(requestMessage->channelId());
		responseMessage->clientHandle(requestMessage->clientHandle());
		responseMessage->statusCode(OpcUaStatusCodeMap::shortString(statusCode));
		opcUaClientManagerIf_->clientManagerMessage(responseMessage);
	}

}
