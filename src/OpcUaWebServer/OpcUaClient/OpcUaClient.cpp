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
#include "OpcUaStackCore/Utility/UniqueId.h"
#include "OpcUaWebServer/OpcUaClient/OpcUaClient.h"

using namespace OpcUaStackCore;
using namespace OpcUaStackClient;

namespace OpcUaWebServer
{

	OpcUaClient::OpcUaClient(void)
	: config_(nullptr)
	, opcUaClientConfig_()
	, state_(S_Close)
	, namespaceMap_()
	, valueInfo_()
	, monitorManager_()
	, opcUaClientIf_(nullptr)

	, polling_(true)
	, slotTimerElement_()

	, serviceSetManager_()
	, sessionService_()
	, attributeService_()
	, subscriptionService_()
	, monitoredItemService_()

	, reconnectTimeout_(10000)
	, subscriptionId_(0)
	{
	}

	OpcUaClient::~OpcUaClient(void)
	{
	}

	void
	OpcUaClient::config(Config* config)
	{
		config_ = config;
	}

	void
	OpcUaClient::opcUaClientConfig(OpcUaClientConfig::SPtr opcUaClientConfig)
	{
		opcUaClientConfig_ = opcUaClientConfig;
	}

	void
	OpcUaClient::opcUaClientIf(OpcUaClientIf* opcUaClientIf)
	{
		opcUaClientIf_ = opcUaClientIf;
	}

	bool
	OpcUaClient::startup(
		const OpcUaClient::SPtr& opcUaClient,
		IOThread::SPtr& ioThread,
		MessageBus::SPtr& messageBus,
		CryptoManager::SPtr& cryptoManager
	)
	{
		OpcUaClientAuthentication& authentication = opcUaClientConfig_->opcUaClientEndpoint_.authentication_;

		Log(Info, "client startup")
			.parameter("DiscoveryUrl", opcUaClientConfig_->opcUaClientEndpoint_.discoveryUrl_)
			.parameter("EndpointUrl", opcUaClientConfig_->opcUaClientEndpoint_.endpointUrl_)
			.parameter("ApplicationUri", opcUaClientConfig_->opcUaClientEndpoint_.applicationUri_)
			.parameter("SecurityMode", MessageSecurityMode::enum2Str(opcUaClientConfig_->opcUaClientEndpoint_.securityMode_))
			.parameter("SecurityPolicy", SecurityPolicy::enum2Str(opcUaClientConfig_->opcUaClientEndpoint_.securityPolicy_))
			.parameter("AuthMode", UserTokenType::enum2Str(authentication.userTokenType_))
			.parameter("PolicyId", authentication.policyId_)
			.parameter("AuthSecurityPolicy", SecurityPolicy::enum2Str(authentication.securityPolicy_));

		ioThread_ = ioThread;
		messageBus_ = messageBus;
		strand_ = ioThread_->createStrand();

		// create value info map
		if (!valueInfo_.addClientConfig(opcUaClientConfig_, opcUaClient)) {
			return false;
		}

		auto sessionStateUpdateCallback = [this](SessionBase& session, SessionServiceStateId sessionState) {
			sessionStateUpdate(session, sessionState);
		};

		// init service sets
		serviceSetManager_.registerIOThread(ioThread->name(), ioThread);

		// create session service
		SessionServiceConfig sessionServiceConfig;
		sessionServiceConfig.secureChannelClient_->discoveryUrl(opcUaClientConfig_->opcUaClientEndpoint_.discoveryUrl_);
		sessionServiceConfig.secureChannelClient_->endpointUrl(opcUaClientConfig_->opcUaClientEndpoint_.endpointUrl_);
		sessionServiceConfig.secureChannelClient_->applicationUri(opcUaClientConfig_->opcUaClientEndpoint_.applicationUri_);
		sessionServiceConfig.secureChannelClient_->cryptoManager(cryptoManager);
		sessionServiceConfig.secureChannelClient_->securityMode(opcUaClientConfig_->opcUaClientEndpoint_.securityMode_);
		sessionServiceConfig.secureChannelClient_->securityPolicy(opcUaClientConfig_->opcUaClientEndpoint_.securityPolicy_);
		sessionServiceConfig.session_->sessionName("ASNeGWebServer");
		sessionServiceConfig.ioThreadName(ioThread_->name());
		sessionServiceConfig.sessionServiceChangeHandler_ = sessionStateUpdateCallback;
		sessionServiceConfig.sessionServiceChangeHandlerStrand_ = strand_;
		sessionServiceConfig.session_->reconnectTimeout(5000);
		sessionServiceConfig.sessionServiceName_ = std::string("SessionService_") + UniqueId::createStringUniqueId();
		if (authentication.userTokenType_ == UserTokenType::EnumAnonymous) {
				sessionServiceConfig.session_->authenticationAnonymous(
						authentication.policyId_);
		} else if (authentication.userTokenType_ == UserTokenType::EnumUserName) {
				sessionServiceConfig.session_->authenticationUserName(
					authentication.policyId_,
					authentication.userName_,
					authentication.password_,
					SecurityPolicy::enum2Str(authentication.securityPolicy_)
				);
		}
		serviceSetManager_.sessionService(sessionServiceConfig);
		sessionService_ = serviceSetManager_.sessionService(sessionServiceConfig);

		// create attribute service
		AttributeServiceConfig attributeServiceConfig;
		attributeServiceConfig.ioThreadName(ioThread_->name());
		attributeServiceConfig.attributeServiceName_ = std::string("AttributeService_") + UniqueId::createStringUniqueId();
		attributeService_ = serviceSetManager_.attributeService(sessionService_, attributeServiceConfig);


		// create subscriptions service
		SubscriptionServiceConfig subscriptionServiceConfig;
		subscriptionServiceConfig.ioThreadName(ioThread_->name());
		subscriptionServiceConfig.subscriptionServiceName_ = std::string("SubscriptionService_") + UniqueId::createStringUniqueId();
		subscriptionServiceConfig.dataChangeNotificationHandler_ = [this](const MonitoredItemNotification::SPtr& monitoredItem) {
			dataChangeNotification(monitoredItem);
		};
		subscriptionServiceConfig.subscriptionStateUpdateHandler_ = [this](SubscriptionState subscriptionState, uint32_t subscriptionId) {
			subscriptionStateUpdate(subscriptionState, subscriptionId);
		};
		subscriptionService_ = serviceSetManager_.subscriptionService(sessionService_, subscriptionServiceConfig);


		// create monitored item service
		MonitoredItemServiceConfig monitoredItemServiceConfig;
		monitoredItemServiceConfig.ioThreadName(ioThread_->name());
		monitoredItemServiceConfig.monitoredItemServiceName_ = std::string("MonitoredItemService_") + UniqueId::createStringUniqueId();
		monitoredItemService_ = serviceSetManager_.monitoredItemService(sessionService_, monitoredItemServiceConfig);


		// connect to server
		sessionService_->asyncConnect();

		slotTimerElement_ = boost::make_shared<SlotTimerElement>();
		slotTimerElement_->timeoutCallback(strand_, boost::bind(&OpcUaClient::timerLoop, this));
		slotTimerElement_->expireTime(boost::posix_time::microsec_clock::local_time(), 900);
		ioThread_->slotTimer()->start(slotTimerElement_);

		return true;
	}

	bool
	OpcUaClient::shutdown(void)
	{
		ioThread_->slotTimer()->stopSlotTimerLoop();
		return true;
	}

	ValueInfoEntry::Map&
	OpcUaClient::valueInfoEntryMap(void)
	{
		return valueInfo_.valueInfoEntryMap();
	}

	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	//
	// SessionIf
	//
	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	void
	OpcUaClient::sessionStateUpdate(SessionBase& session, SessionServiceStateId sessionState)
	{
		switch (sessionState)
		{
			case SessionServiceStateId::Established:
				Log(Debug, "session state changed to connect");
				readNamespaceArray();
				break;
			case SessionServiceStateId::Disconnected:
				Log(Debug, "session state changed to disconnect/close");
				state_ = S_Close;
				if (!polling_) {
					deleteSubscription();
				}
				break;
			default:
				Log(Warning, "session state changed to unexpected conditional");
		}
	}

	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	//
	// handle read request
	//
	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	void
	OpcUaClient::readRequestError(uint32_t channelId, const std::string& clientHandle, OpcUaStatusCode statusCode)
	{
		ReadResponseData readResponseData;
		readResponseData.statusCode(OpcUaStatusCodeMap::shortString(statusCode));

		Message::SPtr responseMessage = boost::make_shared<Message>(Message::MT_ReadResponse);
		responseMessage->channelId(channelId);
		responseMessage->clientHandle(clientHandle);

		boost::property_tree::ptree pt;
		if (!readResponseData.jsonEncode(pt)) {
			responseMessage->statusCode(OpcUaStatusCodeMap::shortString(BadInternalError));
		}
		else {
			responseMessage->body(pt);
		}
		opcUaClientIf_->clientMessage(responseMessage);
	}

	void
	OpcUaClient::readRequest(Message::SPtr& requestMessage, ReadRequestData& readRequestData, const ValueInfoEntry::SPtr& valueInfoEntry)
	{
		// check session state
		if (state_ != S_Open) {
			readRequestError(requestMessage->channelId(), requestMessage->clientHandle(), BadNotConnected);
			return;
		}

		// map namespace index of the node
		NamespaceMap::iterator it;
		OpcUaNodeId nodeId = valueInfoEntry->nodeId_;
		it = namespaceMap_.find(nodeId.namespaceIndex());
		if (it == namespaceMap_.end()) {
			Log(Error, "namespace index mapping error")
				.parameter("VariableName", readRequestData.variable())
				.parameter("NamespaceIndex", nodeId.namespaceIndex());

			readRequestError(requestMessage->channelId(), requestMessage->clientHandle(), BadNodeIdInvalid);
			return;
		}
		nodeId.namespaceIndex(it->second);

		// create opc ua read transaction and send request to the opc ua server
		auto readTrx = boost::make_shared<ServiceTransactionClientRead>();
		readTrx->variable_ = readRequestData.variable();
		readTrx->channelId_ = requestMessage->channelId();
		readTrx->clientHandle_ = requestMessage->clientHandle();
		readTrx->valueInfoEntry_ = valueInfoEntry;
		readTrx->resultHandler(
			[this](ServiceTransactionRead::SPtr& trxRead) {
				attributeServiceReadResponse(trxRead);
			}
		);

		auto trxReq = readTrx->request();
		trxReq->maxAge(0);
		trxReq->timestampsToReturn(2);

		auto readValueId = boost::make_shared<ReadValueId>();
		nodeId.copyTo(*readValueId->nodeId());
		readValueId->attributeId((OpcUaInt32) 13);
		readValueId->dataEncoding().namespaceIndex((OpcUaInt16) 0);
		trxReq->readValueIdArray()->set(readValueId);

		attributeService_->asyncSend(readTrx);
	}

	void
	OpcUaClient::readResponse(ServiceTransactionClientRead::SPtr trxRead)
	{
		auto trxreq = trxRead->request();
		auto trxRes = trxRead->response();

		// error handling
		OpcUaStatusCode statusCode = trxRead->statusCode();
		if (statusCode != Success) {
			Log(Error, "read response error")
				.parameter("VariableName", trxRead->variable_)
				.parameter("Reason", OpcUaStatusCodeMap::shortString(statusCode));

			readRequestError(trxRead->channelId_, trxRead->clientHandle_, statusCode);
			return;
		}

		if (trxRes->dataValueArray()->size() != 1) {
			Log(Error, "read response error, because data array size error")
				.parameter("VariableName", trxRead->variable_);

			readRequestError(trxRead->channelId_, trxRead->clientHandle_, BadInternalError);
			return;
		}

		// create response data
		boost::property_tree::ptree pt;
		OpcUaDataValue::SPtr dataValue;
		trxRes->dataValueArray()->get(0, dataValue);
		ReadResponseData readResponseData;
		readResponseData.dataValue(dataValue);
		readResponseData.valueInfoEntry(trxRead->valueInfoEntry_);
		if (!readResponseData.jsonEncode(pt)) {
			readRequestError(trxRead->channelId_, trxRead->clientHandle_, BadInternalError);
			return;
		}

		// send response
		auto responseMessage = boost::make_shared<Message>(Message::MT_ReadResponse);
		responseMessage->channelId(trxRead->channelId_);
		responseMessage->clientHandle(trxRead->clientHandle_);
		responseMessage->body(pt);
		opcUaClientIf_->clientMessage(responseMessage);
	}

	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	//
	// handle write request
	//
	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	void
	OpcUaClient::writeRequestError(uint32_t channelId, const std::string& clientHandle, OpcUaStatusCode statusCode)
	{
		ReadResponseData readResponseData;
		readResponseData.statusCode(OpcUaStatusCodeMap::shortString(statusCode));

		auto responseMessage = boost::make_shared<Message>(Message::MT_WriteResponse);
		responseMessage->channelId(channelId);
		responseMessage->clientHandle(clientHandle);

		boost::property_tree::ptree pt;
		if (!readResponseData.jsonEncode(pt)) {
			responseMessage->statusCode(OpcUaStatusCodeMap::shortString(BadInternalError));
		}
		else {
			responseMessage->body(pt);
		}
		opcUaClientIf_->clientMessage(responseMessage);
	}

	void
	OpcUaClient::writeRequest(Message::SPtr& requestMessage, WriteRequestData& writeRequestData, const ValueInfoEntry::SPtr& valueInfoEntry)
	{
		// check session state
		if (state_ != S_Open) {
			writeRequestError(requestMessage->channelId(), requestMessage->clientHandle(), BadNotConnected);
			return;
		}

		// map namespace index of the node
		OpcUaNodeId nodeId = valueInfoEntry->nodeId_;
		auto it = namespaceMap_.find(nodeId.namespaceIndex());
		if (it == namespaceMap_.end()) {
			Log(Error, "namespace index mapping error")
				.parameter("VariableName", writeRequestData.variable())
				.parameter("NamespaceIndex", nodeId.namespaceIndex());

			writeRequestError(requestMessage->channelId(), requestMessage->clientHandle(), BadNodeIdInvalid);
			return;
		}
		nodeId.namespaceIndex(it->second);

		// create opc ua write transaction and send request to the opc ua server
		auto writeTrx = boost::make_shared<ServiceTransactionClientWrite>();
		writeTrx->variable_ = writeRequestData.variable();
		writeTrx->channelId_ = requestMessage->channelId();
		writeTrx->clientHandle_ = requestMessage->clientHandle();
		writeTrx->resultHandler(
			[this](ServiceTransactionWrite::SPtr& trxWrite) {
				attributeServiceWriteResponse(trxWrite);
			}
		);

		auto trxReq = writeTrx->request();

		auto writeValue = boost::make_shared<WriteValue>();
		nodeId.copyTo(*writeValue->nodeId());
		writeValue->attributeId((OpcUaInt32) 13);
		writeValue->dataValue(*writeRequestData.dataValue());

		trxReq->writeValueArray()->resize(1);
		trxReq->writeValueArray()->set(writeValue);

		attributeService_->asyncSend(writeTrx);
	}

	void
	OpcUaClient::writeResponse(ServiceTransactionClientWrite::SPtr trxWrite)
	{
		auto trxreq = trxWrite->request();
		auto trxRes = trxWrite->response();

		// error handling
		OpcUaStatusCode statusCode = trxWrite->statusCode();
		if (statusCode != Success) {
			Log(Error, "write response error")
				.parameter("VariableName", trxWrite->variable_)
				.parameter("Reason", OpcUaStatusCodeMap::shortString(statusCode));

			writeRequestError(trxWrite->channelId_, trxWrite->clientHandle_, statusCode);
			return;
		}

		if (trxRes->results()->size() != 1) {
			Log(Error, "write response error, because result array size error")
				.parameter("VariableName", trxWrite->variable_);

			writeRequestError(trxWrite->channelId_, trxWrite->clientHandle_, BadInternalError);
			return;
		}

		// create response data
		trxRes->results()->get(0, statusCode);

		// send response
		auto responseMessage = boost::make_shared<Message>(Message::MT_WriteResponse);
		responseMessage->channelId(trxWrite->channelId_);
		responseMessage->clientHandle(trxWrite->clientHandle_);
		if (statusCode != Success) responseMessage->statusCode(OpcUaStatusCodeMap::shortString(statusCode));
		opcUaClientIf_->clientMessage(responseMessage);
	}

	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	//
	// handle monitoring
	//
	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	void
	OpcUaClient::monitorStartRequestError(uint32_t channelId, const std::string& clientHandle, OpcUaStatusCode statusCode)
	{
		MonitorStartResponseData monitorStartResponseData;
		monitorStartResponseData.statusCode(OpcUaStatusCodeMap::shortString(statusCode));

		auto responseMessage = boost::make_shared<Message>(Message::MT_MonitorStartResponse);
		responseMessage->channelId(channelId);
		responseMessage->clientHandle(clientHandle);

		boost::property_tree::ptree pt;
		if (!monitorStartResponseData.jsonEncode(pt)) {
			responseMessage->statusCode(OpcUaStatusCodeMap::shortString(BadInternalError));
		}
		else {
			responseMessage->body(pt);
		}
		opcUaClientIf_->clientMessage(responseMessage);
	}

	void
	OpcUaClient::monitorStartRequest(Message::SPtr& requestMessage, MonitorStartRequestData& monitorStartRequestData, const ValueInfoEntry::SPtr& valueInfoEntry)
	{
		uint32_t channelId = requestMessage->channelId();
		std::string variable = requestMessage->clientHandle(); //  FIXME: use variable

		if (!monitorManager_.addMonitorItem(channelId, variable, valueInfoEntry)) {
			// send response
			monitorStartRequestError(requestMessage->channelId(), requestMessage->clientHandle(), BadEntryExists);
			return;
		}

		// send response
		auto responseMessage = boost::make_shared<Message>(Message::MT_MonitorStartResponse);
		responseMessage->channelId(requestMessage->channelId());
		responseMessage->clientHandle(requestMessage->clientHandle());
		opcUaClientIf_->clientMessage(responseMessage);
	}

	void
	OpcUaClient::monitorStopRequestError(uint32_t channelId, const std::string& clientHandle, OpcUaStatusCode statusCode)
	{
		MonitorStopResponseData monitorStopResponseData;
		monitorStopResponseData.statusCode(OpcUaStatusCodeMap::shortString(statusCode));

		auto responseMessage = boost::make_shared<Message>(Message::MT_MonitorStopResponse);
		responseMessage->channelId(channelId);
		responseMessage->clientHandle(clientHandle);

		boost::property_tree::ptree pt;
		if (!monitorStopResponseData.jsonEncode(pt)) {
			responseMessage->statusCode(OpcUaStatusCodeMap::shortString(BadInternalError));
		}
		else {
			responseMessage->body(pt);
		}
		opcUaClientIf_->clientMessage(responseMessage);
	}

	void
	OpcUaClient::monitorStopRequest(Message::SPtr& requestMessage, MonitorStopRequestData& monitorStopRequestData, const ValueInfoEntry::SPtr& valueInfoEntry)
	{
		uint32_t channelId = requestMessage->channelId();
		std::string variable = requestMessage->clientHandle(); //  FIXME: use variable

		if (!monitorManager_.delMonitorItem(channelId, variable)) {
			// send response
			monitorStopRequestError(requestMessage->channelId(), requestMessage->clientHandle(), BadNoEntryExists);
			return;
		}

		// send response
		auto responseMessage = boost::make_shared<Message>(Message::MT_MonitorStopResponse);
		responseMessage->channelId(requestMessage->channelId());
		responseMessage->clientHandle(requestMessage->clientHandle());
		opcUaClientIf_->clientMessage(responseMessage);
	}

	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	//
	// handle historical read request
	//
	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	void
	OpcUaClient::historicalReadRequest(Message::SPtr& message, HistoricalReadRequestData& historicalReadRequestData, const ValueInfoEntry::SPtr& valueInfoEntry)
	{
		// FIXME: todo
	}

	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	//
	// management functions
	//
	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	void
	OpcUaClient::closeChannelMessage(Message::SPtr& message)
	{
		monitorManager_.delMonitorItem(message->channelId());
	}

	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	//
	// handle namespace array
	//
	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	void
	OpcUaClient::readNamespaceArray(void)
	{
		auto readTrx = boost::make_shared<ServiceTransactionClientRead>();
		auto req = readTrx->request();
		req->maxAge(0);
		req->timestampsToReturn(2);

		ReadValueId::SPtr readValueId = boost::make_shared<ReadValueId>();
		readValueId->nodeId((OpcUaInt16)0, (OpcUaInt32)2255);
		readValueId->attributeId((OpcUaInt32) 13);
		readValueId->dataEncoding().namespaceIndex((OpcUaInt16) 0);
		req->readValueIdArray()->set(readValueId);

		readTrx->resultHandler(
			[this](ServiceTransactionRead::SPtr& serviceTransactionRead) {
				handleNamespaceArray(serviceTransactionRead);
			}
		);
		attributeService_->asyncSend(readTrx);
	}

	void
	OpcUaClient::handleNamespaceArray(ServiceTransactionRead::SPtr serviceTransactionRead)
	{
		auto req = serviceTransactionRead->request();
		auto res = serviceTransactionRead->response();

		// error handling
		OpcUaStatusCode statusCode = serviceTransactionRead->statusCode();
		if (statusCode != Success) {
			Log(Error, "read namespace array transaction error")
				.parameter("StatusCode", OpcUaStatusCodeMap::shortString(statusCode));
			return;
		}
		if (res->dataValueArray()->size() != 1) {
			Log(Error, "read namespace array response error, because data array size error");
			return;
		}

		// get data value
		OpcUaDataValue::SPtr dataValue;
		res->dataValueArray()->get(0, dataValue);

		if (dataValue->statusCode() != Success) {
			Log(Error, "read namespace array data value error")
				.parameter("StatusCode", OpcUaStatusCodeMap::shortString(statusCode));
			return;
		}
		OpcUaVariant::SPtr variant = dataValue->variant();


		// get namespace array
		std::map<std::string, uint32_t> namespaceMap;
		Log(Debug, "read namespace array from server")
		    .parameter("EndpointUrl", opcUaClientConfig_->opcUaClientEndpoint_.endpointUrl_);
		for (int32_t idx=0; idx < variant->arrayLength(); idx++) {
			std::string uri = variant->variant()[idx].variantSPtr<OpcUaString>()->value();
			Log(Debug, "")
				.parameter("Uri", uri)
				.parameter("NamespaceIndex", idx);
			namespaceMap.insert(std::make_pair(uri, idx));
		}

		// create namespace mapping
		namespaceMap_.clear();
		for (uint32_t idx=0; idx<opcUaClientConfig_->namespaceUri_.uriVec_.size(); idx++) {
			uint32_t namespaceIndex = idx+1;
			std::string namespaceName = opcUaClientConfig_->namespaceUri_.uriVec_[idx];

			auto it = namespaceMap.find(namespaceName);
			if (it == namespaceMap.end()) {
				Log(Error, "namespace name not exist on server")
				    .parameter("EndpointUrl", opcUaClientConfig_->opcUaClientEndpoint_.endpointUrl_)
				    .parameter("NamespaceName", namespaceName);
				return;
			}

			namespaceMap_.insert(std::make_pair(namespaceIndex, it->second));
		}

		Log(Debug, "session state changed to open");
		state_ = S_Open;

		if (polling_) return;
		openSubscription();
	}

	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	//
	// AttributeServiceIf
	//
	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	void
	OpcUaClient::attributeServiceReadResponse(ServiceTransactionRead::SPtr serviceTransactionRead)
	{
		auto trxRead = boost::static_pointer_cast<ServiceTransactionClientRead>(serviceTransactionRead);
		if (trxRead->channelId_ == 0) {
			handleNamespaceArray(serviceTransactionRead);
			return;
		}

		// handle read response
		if (trxRead->polling_ == true) {
			readResponse1(trxRead);
		}
		else {
			readResponse(trxRead);
		}
	}

	void
	OpcUaClient::attributeServiceWriteResponse(ServiceTransactionWrite::SPtr serviceTransactionWrite)
	{
		auto trxWrite = boost::static_pointer_cast<ServiceTransactionClientWrite>(serviceTransactionWrite);

		// handle write response
		writeResponse(trxWrite);
	}

	void
	OpcUaClient::attributeServiceHistoryReadResponse(ServiceTransactionHistoryRead::SPtr serviceTransactionHistoryRead)
	{
		std::cout << "attributeServiceHistoryReadResponse" << std::endl;
	}

	void
	OpcUaClient::attributeServiceHistoryUpdateResponse(ServiceTransactionHistoryUpdate::SPtr serviceTransactionHistoryUpdate)
	{
		std::cout << "attributeServiceHistoryUpdateResponse" << std::endl;
	}

	void
	OpcUaClient::timerLoop(void)
	{
		if (polling_) {
			polling();
		}
		else {
			createMonitoredItems();
		}
	}

	void
	OpcUaClient::polling(void)
	{
		// check session state
		if (state_ != S_Open) {
			return;
		}

		for (auto it = monitorManager_.monitorItemMap().begin(); it != monitorManager_.monitorItemMap().end(); it++) {
			auto monitorItem = it->second;
			auto valueInfoEntry = monitorItem->valueInfoEntry();

			// map namepsace index of the node
			NamespaceMap::iterator it1;
			OpcUaNodeId nodeId = valueInfoEntry->nodeId_;
			it1 = namespaceMap_.find(nodeId.namespaceIndex());
			if (it1 == namespaceMap_.end()) {
				continue;
			}
			nodeId.namespaceIndex(it1->second);

			// create opc ua read transaction and send request to the opc ua server
			auto readTrx = boost::make_shared<ServiceTransactionClientRead>();
			readTrx->variable_ = it->first;
			readTrx->channelId_ = 1;
			readTrx->clientHandle_ = it->first;
			readTrx->valueInfoEntry_ = valueInfoEntry;

			readTrx->polling_ = true;
			readTrx->channelIdSet_ = monitorItem->channelIdSet();

			auto trxReq = readTrx->request();
			trxReq->maxAge(0);
			trxReq->timestampsToReturn(2);

			auto readValueId = boost::make_shared<ReadValueId>();
			nodeId.copyTo(*readValueId->nodeId());
			readValueId->attributeId((OpcUaInt32) 13);
			readValueId->dataEncoding().namespaceIndex((OpcUaInt16) 0);
			trxReq->readValueIdArray()->set(readValueId);

			readTrx->resultHandler(
				[this](ServiceTransactionRead::SPtr& serviceTransactionRead) {
					attributeServiceReadResponse(serviceTransactionRead);
				}
			);
			attributeService_->asyncSend(readTrx);
		}
	}

	void
	OpcUaClient::readResponse1(ServiceTransactionClientRead::SPtr trxRead)
	{
		auto trxreq = trxRead->request();
		auto trxRes = trxRead->response();

		// error handling
		OpcUaStatusCode statusCode = trxRead->statusCode();
		if (statusCode != Success) {
			for (auto it = trxRead->channelIdSet_.begin(); it !=  trxRead->channelIdSet_.end(); it++) {
				auto responseMessage = boost::make_shared<Message>(Message::MT_MonitorUpdateMessage);
				responseMessage->channelId(*it);
				responseMessage->clientHandle(trxRead->clientHandle_);
				responseMessage->statusCode(OpcUaStatusCodeMap::shortString(statusCode));
				opcUaClientIf_->clientMessage(responseMessage);
			}
			return;
		}

		if (trxRes->dataValueArray()->size() != 1) {
			for (auto it = trxRead->channelIdSet_.begin(); it !=  trxRead->channelIdSet_.end(); it++) {
				auto responseMessage = boost::make_shared<Message>(Message::MT_MonitorUpdateMessage);
				responseMessage->channelId(*it);
				responseMessage->clientHandle(trxRead->clientHandle_);
				responseMessage->statusCode(OpcUaStatusCodeMap::shortString(BadInternalError));
				opcUaClientIf_->clientMessage(responseMessage);
			}
			return;
		}

		for (auto it = trxRead->channelIdSet_.begin(); it !=  trxRead->channelIdSet_.end(); it++) {
			// create response data
			boost::property_tree::ptree pt;
			OpcUaDataValue::SPtr dataValue;
			trxRes->dataValueArray()->get(0, dataValue);
			ReadResponseData readResponseData;
			readResponseData.dataValue(dataValue);
			readResponseData.valueInfoEntry(trxRead->valueInfoEntry_);
			if (!readResponseData.jsonEncode(pt)) {
				auto responseMessage = boost::make_shared<Message>(Message::MT_MonitorUpdateMessage);
				responseMessage->channelId(*it);
				responseMessage->clientHandle(trxRead->clientHandle_);
				responseMessage->statusCode(OpcUaStatusCodeMap::shortString(BadInternalError));
				opcUaClientIf_->clientMessage(responseMessage);
				return;
			}

			// send response
			auto responseMessage = boost::make_shared<Message>(Message::MT_MonitorUpdateMessage);
			responseMessage->channelId(*it);
			responseMessage->clientHandle(trxRead->clientHandle_);
			responseMessage->body(pt);
			opcUaClientIf_->clientMessage(responseMessage);
		}
	}


	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	//
	// SubscriptionService
	//
	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	void
	OpcUaClient::openSubscription(void)
	{
		if (subscriptionId_ != 0) deleteSubscription();
		else createSubscription();
	}

	void
	OpcUaClient::createSubscription(void)
	{
		auto trx = boost::make_shared<ServiceTransactionCreateSubscription>();

		trx->resultHandler(
			[this](ServiceTransactionCreateSubscription::SPtr& serviceTransactionCreateSubscription) {
				subscriptionServiceCreateSubscriptionResponse(serviceTransactionCreateSubscription);
			}
		);
		subscriptionService_->asyncSend(trx);
	}

	void
	OpcUaClient::deleteSubscription(void)
	{
	    // deactivate monitored items
		boost::posix_time::ptime now = boost::posix_time::microsec_clock::universal_time();
		boost::posix_time::ptime reconnectTime = now + boost::posix_time::millisec(reconnectTimeout_);
		for (auto it = monitorManager_.monitorItemMap().begin(); it != monitorManager_.monitorItemMap().end(); it++) {
			auto monitorItem = it->second;

			monitorItem->reconnectTime(reconnectTime);
			monitorItem->monitorItemState(MonitorItem::MIS_Close);
		}

		auto trx = boost::make_shared<ServiceTransactionDeleteSubscriptions>();
		auto req = trx->request();
		req->subscriptionIds()->resize(1);
		req->subscriptionIds()->set(0, subscriptionId_);

		trx->resultHandler(
			[this](ServiceTransactionDeleteSubscriptions::SPtr& serviceTransactionDeleteSubscriptions) {
				subscriptionServiceDeleteSubscriptionsResponse(serviceTransactionDeleteSubscriptions);
			}
		);
		subscriptionService_->asyncSend(trx);
	}

	void
	OpcUaClient::subscriptionServiceCreateSubscriptionResponse(ServiceTransactionCreateSubscription::SPtr trx)
	{
		auto res = trx->response();
		if (trx->responseHeader()->serviceResult() != Success) {
			Log(Error, "create subscription response error")
				.parameter("StatusCode", OpcUaStatusCodeMap::shortString(trx->responseHeader()->serviceResult()));
			return;
		}

		subscriptionId_ = res->subscriptionId();
		Log(Debug, "open subscription")
		    .parameter("SubscriptionId", subscriptionId_);

		createMonitoredItems();
	}

    void
    OpcUaClient::subscriptionServiceModifySubscriptionResponse(ServiceTransactionModifySubscription::SPtr serviceTransactionModifySubscription)
    {
    }

    void
    OpcUaClient::subscriptionServiceTransferSubscriptionsResponse(ServiceTransactionTransferSubscriptions::SPtr serviceTransactionTransferSubscriptions)
    {
    }

    void
    OpcUaClient::subscriptionServiceDeleteSubscriptionsResponse(ServiceTransactionDeleteSubscriptions::SPtr serviceTransactionDeleteSubscriptions)
    {
		Log(Debug, "close subscription")
		    .parameter("SubscriptionId", subscriptionId_);

    	subscriptionId_ = 0;

		// create new subscription
    	createSubscription();
    }

	void
	OpcUaClient::dataChangeNotification(const MonitoredItemNotification::SPtr& monitoredItem)
	{
		std::cout << "Data update ..." << std::endl;
		assert(monitoredItem.get() == nullptr);

		// locate the associated entry
		auto monitorItem = monitorManager_.getMonitorItem(monitoredItem->clientHandle());
		if (monitorItem.get() == nullptr) {
			Log(Warning, "monitor item no found in data change notification")
				.parameter("ClientHandle", monitoredItem->clientHandle());
			return;
		}

		for (auto it = monitorItem->channelIdSet().begin(); it != monitorItem->channelIdSet().end(); it++) {
			auto updateMessage = boost::make_shared<Message>(Message::MT_MonitorUpdateMessage);
			updateMessage->channelId(*it);
			updateMessage->clientHandle(monitorItem->valueInfoEntry()->valueName_);

			OpcUaStatusCode statusCode = monitoredItem->value().statusCode();
			if (statusCode == Success) {
				ReadResponseData readResponseData;
				readResponseData.dataValue(monitoredItem->value());
				readResponseData.valueInfoEntry(monitorItem->valueInfoEntry());

				boost::property_tree::ptree pt;
				if (readResponseData.jsonEncode(pt)) {
					updateMessage->body(pt);
				}
				else {
					Log(Error, "data encoder error in data change notification")
						.parameter("ClientHandle", monitoredItem->clientHandle())
						.parameter("ApplId", monitorItem->valueInfoEntry()->valueName_);
					statusCode = BadInternalError;
				}
			}

			if (statusCode != Success) {
				updateMessage->statusCode(OpcUaStatusCodeMap::shortString(statusCode));
			}

			opcUaClientIf_->clientMessage(updateMessage);
		}
	}

	void
	OpcUaClient::subscriptionStateUpdate(SubscriptionState subscriptionState, uint32_t subscriptionId)
	{
		std::cout << "Subscription update ..." << subscriptionId << " " << subscriptionState << std::endl;

		// deleteSubscription
	}

	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	//
	// MonitoredItemService
	//
	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	void
	OpcUaClient::createMonitoredItems(MonitorItem::Vec& monitorItemVec)
	{
		if (state_ != S_Open) return;
		if (subscriptionId_ == 0) return;
		if (monitorItemVec.size() == 0) return;

		Log(Debug, "create monitored items")
		    .parameter("Size", monitorItemVec.size());

		// create monitored item transaction
		auto trx = boost::make_shared<ServiceTransactionCreateMonitoredItems>();
		auto req = trx->request();
		req->subscriptionId(subscriptionId_);
		req->itemsToCreate()->resize(monitorItemVec.size());

		uint32_t idx = 0;
		for (auto it = monitorItemVec.begin(); it !=  monitorItemVec.end(); it++) {
			auto monitorItem = *it;
			auto valueInfoEntry = monitorItem->valueInfoEntry();

			// map namepsace index of the node
			OpcUaNodeId nodeId = valueInfoEntry->nodeId_;
			auto it1 = namespaceMap_.find(nodeId.namespaceIndex());
			if (it1 == namespaceMap_.end()) {
				continue;
			}
			nodeId.namespaceIndex(it1->second);

			Log(Debug, "Try to create monitored item")
			    .parameter("Idx", idx)
			    .parameter("NodeId", monitorItem->valueInfoEntry()->nodeId_.toString())
			    .parameter("ApplId", monitorItem->valueInfoEntry()->valueName_)
			    .parameter("ClientHandle", monitorItem->clientHandle());

			auto monitoredItemCreateRequest = boost::make_shared<MonitoredItemCreateRequest>();
			monitoredItemCreateRequest->itemToMonitor().nodeId()->copyFrom(nodeId);
			monitoredItemCreateRequest->requestedParameters().clientHandle(monitorItem->clientHandle());
			req->itemsToCreate()->push_back(monitoredItemCreateRequest);
			idx++;
		}

		trx->resultHandler(
			[this](ServiceTransactionCreateMonitoredItems::SPtr& serviceTransactionCreateMonitoredItems) {
				monitoredItemServiceCreateMonitoredItemsResponse(serviceTransactionCreateMonitoredItems);
			}
		);
		monitoredItemService_->asyncSend(trx);
	}

	void
	OpcUaClient::createMonitoredItems(void)
	{
		if (state_ != S_Open) return;
		if (subscriptionId_ == 0) return;

		boost::posix_time::ptime now = boost::posix_time::microsec_clock::universal_time();
		boost::posix_time::ptime reconnectTime = now + boost::posix_time::millisec(reconnectTimeout_);
		MonitorItem::Vec monitorItemVec;

		for (auto it = monitorManager_.monitorItemMap().begin(); it != monitorManager_.monitorItemMap().end(); it++) {
			MonitorItem::SPtr monitorItem = it->second;

			if (now <= monitorItem->reconnectTime()) continue;
			if (monitorItem->monitorItemState() != MonitorItem::MIS_Close) continue;
			monitorItem->reconnectTime(reconnectTime);
			monitorItemVec.push_back(monitorItem);
		}

		createMonitoredItems(monitorItemVec);
	}

	void
    OpcUaClient::monitoredItemServiceCreateMonitoredItemsResponse(ServiceTransactionCreateMonitoredItems::SPtr serviceTransactionCreateMonitoredItems)
    {
		OpcUaStatusCode statusCode = serviceTransactionCreateMonitoredItems->statusCode();
		if (statusCode != Success) {
			Log(Error, "create monitor item request error")
				.parameter("StatusCode", OpcUaStatusCodeMap::shortString(statusCode));
			return;
		}

		auto req = serviceTransactionCreateMonitoredItems->request();
		auto res = serviceTransactionCreateMonitoredItems->response();

		if (req->itemsToCreate()->size() != res->results()->size()) {
			Log(Error, "create monitored item response error (size)")
				.parameter("ReqSize", req->itemsToCreate()->size())
				.parameter("ResSize", res->results()->size());
			return;
		}


		for (uint32_t idx=0; idx < req->itemsToCreate()->size(); idx++) {
			MonitoredItemCreateRequest::SPtr monitoredItemCreateRequest;
			req->itemsToCreate()->get(idx, monitoredItemCreateRequest);
			if (monitoredItemCreateRequest.get() == nullptr) {
				Log(Error, "create monitored item response error, because request content not found")
					.parameter("Idx", idx);
				continue;
			}

			uint32_t clientHandle = monitoredItemCreateRequest->requestedParameters().clientHandle();
			auto monitorItem = monitorManager_.getMonitorItem(clientHandle);
			if (monitorItem.get() == nullptr) {
				Log(Error, "create monitored item response error, because monitor item not found")
					.parameter("Idx", idx)
					.parameter("ClientHandle", clientHandle);
				continue;
			}

			MonitoredItemCreateResult::SPtr monitoredItemCreateResult;
			res->results()->get(idx, monitoredItemCreateResult);
			if (monitoredItemCreateResult.get() == nullptr) {
				Log(Error, "create monitored item response error, because response content not found")
					.parameter("Idx", idx)
				    .parameter("NodeId", monitorItem->valueInfoEntry()->nodeId_.toString())
				    .parameter("ApplId", monitorItem->valueInfoEntry()->valueName_)
				    .parameter("ClientHandle", monitorItem->clientHandle());
				continue;
			}
			if (monitoredItemCreateResult->statusCode() != Success) {
				Log(Error, "create monitored item response error, because server response error")
					.parameter("Idx", idx)
				    .parameter("NodeId", monitorItem->valueInfoEntry()->nodeId_.toString())
				    .parameter("ApplId", monitorItem->valueInfoEntry()->valueName_)
				    .parameter("ClientHandle", monitorItem->clientHandle())
				    .parameter("StatusCode", monitoredItemCreateResult->statusCode().toString());
				continue;
			}

			Log(Error, "create monitored item")
				.parameter("Idx", idx)
				.parameter("NodeId", monitorItem->valueInfoEntry()->nodeId_.toString())
				.parameter("ApplId", monitorItem->valueInfoEntry()->valueName_)
				.parameter("ClientHandle", monitorItem->clientHandle())
				.parameter("MonitoredItemId", monitoredItemCreateResult->monitoredItemId());
			monitorItem->monitoredItemId(monitoredItemCreateResult->monitoredItemId());
			monitorItem->monitorItemState(MonitorItem::MIS_Open);
		}
    }

	void
	OpcUaClient::deleteMonitoredItems(MonitorItem::Vec& monitorItemVec)
	{
		if (subscriptionId_ == 0) return;
		if (monitorItemVec.size() == 0) return;

		Log(Debug, "delete monitored items")
		    .parameter("Size", monitorItemVec.size());

		// delete monitored item transaction
		auto trx = boost::make_shared<ServiceTransactionDeleteMonitoredItems>();
		auto req = trx->request();
		req->subscriptionId(subscriptionId_);
		req->monitoredItemIds()->resize(monitorItemVec.size());

		uint32_t idx = 0;
		for (auto it = monitorItemVec.begin(); it !=  monitorItemVec.end(); it++) {
			auto monitorItem = *it;
			auto valueInfoEntry = monitorItem->valueInfoEntry();

			// map namepsace index of the node
			OpcUaNodeId nodeId = valueInfoEntry->nodeId_;
			auto it1 = namespaceMap_.find(nodeId.namespaceIndex());
			if (it1 == namespaceMap_.end()) {
				continue;
			}
			nodeId.namespaceIndex(it1->second);

			Log(Debug, "Try to delete monitored item")
			    .parameter("Idx", idx)
			    .parameter("NodeId", monitorItem->valueInfoEntry()->nodeId_.toString())
			    .parameter("ApplId", monitorItem->valueInfoEntry()->valueName_)
			    .parameter("monitoredItemId", monitorItem->monitoredItemId())
			    .parameter("ClientHandle", monitorItem->clientHandle());

			req->monitoredItemIds()->push_back(monitorItem->monitoredItemId());
			idx++;
		}

		trx->resultHandler(
			[this](ServiceTransactionDeleteMonitoredItems::SPtr& serviceTransactionDeleteMonitoredItems) {
			monitoredItemServiceDeleteMonitoredItemsResponse(serviceTransactionDeleteMonitoredItems);
			}
		);
		monitoredItemService_->asyncSend(trx);
	}

    void
    OpcUaClient::monitoredItemServiceDeleteMonitoredItemsResponse(ServiceTransactionDeleteMonitoredItems::SPtr serviceTransactionDeleteMonitoredItems)
    {
		OpcUaStatusCode statusCode = serviceTransactionDeleteMonitoredItems->statusCode();
		if (statusCode != Success) {
			Log(Error, "delete monitor item request error")
				.parameter("StatusCode", OpcUaStatusCodeMap::shortString(statusCode));
			return;
		}

		auto req = serviceTransactionDeleteMonitoredItems->request();
		auto res = serviceTransactionDeleteMonitoredItems->response();

		if (req->monitoredItemIds()->size() != res->results()->size()) {
			Log(Error, "delete monitored item response error (size)")
				.parameter("ReqSize", req->monitoredItemIds()->size())
				.parameter("ResSize", res->results()->size());
			return;
		}


		for (uint32_t idx=0; idx < req->monitoredItemIds()->size(); idx++) {
			uint32_t monitoredItem;
			req->monitoredItemIds()->get(idx, monitoredItem);

			OpcUaStatusCode statusCode;;
			res->results()->get(idx, statusCode);
			if (statusCode != Success) {
				Log(Error, "delete monitored item response error, because server response error")
					.parameter("Idx", idx)
				    .parameter("MonitoredItemId", monitoredItem)
				    .parameter("StatusCode", OpcUaStatusCodeMap::shortString(statusCode));
				continue;
			}

			Log(Error, "delete monitored item")
				.parameter("Idx", idx)
				.parameter("MonitoredItemId", monitoredItem);
		}
    }

    void
    OpcUaClient::monitoredItemServiceModifyMonitoredItemsResponse(ServiceTransactionModifyMonitoredItems::SPtr serviceTransactionModifyMonitoredItems)
    {
    }

    void
    OpcUaClient::monitoredItemServiceSetMonitoringModeResponse(ServiceTransactionSetMonitoringMode::SPtr serviceTransactionSetMonitoringMode)
    {
    }

    void
    OpcUaClient::monitoredItemServiceSetTriggeringResponse(ServiceTransactionSetTriggering::SPtr serviceTransactionSetTriggering)
    {
    }

}
