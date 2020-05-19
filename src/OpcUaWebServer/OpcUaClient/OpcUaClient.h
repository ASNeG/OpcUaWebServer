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

#ifndef __OpcUaWebServer_OpcUaClient_h__
#define __OpcUaWebServer_OpcUaClient_h__

#include <map>
#include "OpcUaStackCore/Base/Config.h"
#include "OpcUaStackClient/ServiceSet/ServiceSetManager.h"
#include "OpcUaWebServer/OpcUaClient/OpcUaClientConfig.h"
#include "OpcUaWebServer/OpcUaClient/ValueInfo.h"
#include "OpcUaWebServer/OpcUaClient/OpcUaClientIf.h"
#include "OpcUaWebServer/OpcUaClient/MonitorManager.h"
#include "OpcUaWebServer/OpcUaClient/ServiceTransactionClient.h"
#include "OpcUaWebServer/OpcUaClient/ReadRequestData.h"
#include "OpcUaWebServer/OpcUaClient/ReadResponseData.h"
#include "OpcUaWebServer/OpcUaClient/WriteRequestData.h"
#include "OpcUaWebServer/OpcUaClient/MonitorStartRequestData.h"
#include "OpcUaWebServer/OpcUaClient/MonitorStartResponseData.h"
#include "OpcUaWebServer/OpcUaClient/MonitorStopRequestData.h"
#include "OpcUaWebServer/OpcUaClient/MonitorStopResponseData.h"
#include "OpcUaWebServer/OpcUaClient/HistoricalReadRequestData.h"
#include "OpcUaWebServer/OpcUaClient/HistoricalReadResponseData.h"
#include "OpcUaWebServer/Messages/Message.h"

namespace OpcUaWebServer
{

	class OpcUaClient
	: public OpcUaStackCore::Object
	{
	  public:
		typedef boost::shared_ptr<OpcUaClient> SPtr;
		typedef std::map<std::string, OpcUaClient::SPtr> Map;

		typedef enum {
			S_Open,
			S_Close
		} State;

		OpcUaClient(void);
		~OpcUaClient(void);

		bool startup(
			const OpcUaClient::SPtr& opcUaClient,
			OpcUaStackCore::IOThread::SPtr& ioThread,
			OpcUaStackCore::MessageBus::SPtr& messageBus,
			OpcUaStackCore::CryptoManager::SPtr& cryptoManager
		);
		bool shutdown(void);

		void config(OpcUaStackCore::Config* config);
		void opcUaClientConfig(OpcUaClientConfig::SPtr opcUaClientConfig);
		void opcUaClientIf(OpcUaClientIf* opcUaClientIf);

		//- SessionServiceIf --------------------------------------------------
		void sessionStateUpdate(OpcUaStackClient::SessionBase& session, OpcUaStackClient::SessionServiceStateId sessionState);
		//- SessionServiceIf --------------------------------------------------

		//- AttributeServiceIf ------------------------------------------------
		void attributeServiceReadResponse(OpcUaStackCore::ServiceTransactionRead::SPtr serviceTransactionRead);
		void attributeServiceWriteResponse(OpcUaStackCore::ServiceTransactionWrite::SPtr serviceTransactionWrite);
		void attributeServiceHistoryReadResponse(OpcUaStackCore::ServiceTransactionHistoryRead::SPtr serviceTransactionHistoryRead);
		void attributeServiceHistoryUpdateResponse(OpcUaStackCore::ServiceTransactionHistoryUpdate::SPtr serviceTransactionHistoryUpdate);
		//- AttributeServuceIf ------------------------------------------------

		//- SubscriptionServiceIf ---------------------------------------------
	    void subscriptionServiceCreateSubscriptionResponse(OpcUaStackCore::ServiceTransactionCreateSubscription::SPtr serviceTransactionCreateSubscription);
	    void subscriptionServiceModifySubscriptionResponse(OpcUaStackCore::ServiceTransactionModifySubscription::SPtr serviceTransactionModifySubscription);
	    void subscriptionServiceTransferSubscriptionsResponse(OpcUaStackCore::ServiceTransactionTransferSubscriptions::SPtr serviceTransactionTransferSubscriptions);
	    void subscriptionServiceDeleteSubscriptionsResponse(OpcUaStackCore::ServiceTransactionDeleteSubscriptions::SPtr serviceTransactionDeleteSubscriptions);

		void dataChangeNotification(const OpcUaStackCore::MonitoredItemNotification::SPtr& monitoredItem);
		void subscriptionStateUpdate(OpcUaStackClient::SubscriptionState subscriptionState, uint32_t subscriptionId);
		//- SubscriptionServiceIf ---------------------------------------------

		//- MonitoredItemServiceIf --------------------------------------------
	    void monitoredItemServiceCreateMonitoredItemsResponse(OpcUaStackCore::ServiceTransactionCreateMonitoredItems::SPtr serviceTransactionCreateMonitoredItems);
	    void monitoredItemServiceDeleteMonitoredItemsResponse(OpcUaStackCore::ServiceTransactionDeleteMonitoredItems::SPtr serviceTransactionDeleteMonitoredItems);
	    void monitoredItemServiceModifyMonitoredItemsResponse(OpcUaStackCore::ServiceTransactionModifyMonitoredItems::SPtr serviceTransactionModifyMonitoredItems);
	    void monitoredItemServiceSetMonitoringModeResponse(OpcUaStackCore::ServiceTransactionSetMonitoringMode::SPtr serviceTransactionSetMonitoringMode);
	    void monitoredItemServiceSetTriggeringResponse(OpcUaStackCore::ServiceTransactionSetTriggering::SPtr serviceTransactionSetTriggering);
		//- MonitoredItemServiceIf --------------------------------------------

		ValueInfoEntry::Map& valueInfoEntryMap(void);
		void readRequest(Message::SPtr& message, ReadRequestData& readRequestData, const ValueInfoEntry::SPtr& valueInfoEntry);
		void writeRequest(Message::SPtr& message, WriteRequestData& writeRequestData, const ValueInfoEntry::SPtr& valueInfoEntry);
		void monitorStartRequest(Message::SPtr& message, MonitorStartRequestData& monitorStartRequestData, const ValueInfoEntry::SPtr& valueInfoEntry);
		void monitorStopRequest(Message::SPtr& message, MonitorStopRequestData& monitorStopRequestData, const ValueInfoEntry::SPtr& valueInfoEntry);
		void historicalReadRequest(Message::SPtr& message, HistoricalReadRequestData& historicalReadRequestData, const ValueInfoEntry::SPtr& valueInfoEntry);
		void closeChannelMessage(Message::SPtr& message);

	  private:
		void readNamespaceArray(void);
		void openSubscription(void);
		void createSubscription(void);
		void deleteSubscription(void);
		void createMonitoredItems(void);
		void createMonitoredItems(MonitorItem::Vec& monitorItemVec);
		void deleteMonitoredItems(MonitorItem::Vec& monitorItemVec);
		void handleNamespaceArray(OpcUaStackCore::ServiceTransactionRead::SPtr serviceTransactionRead);
		void readRequestError(uint32_t channelId, const std::string& clientHandle, OpcUaStackCore::OpcUaStatusCode statusCode);
		void readResponse(ServiceTransactionClientRead::SPtr serviceTransactionRead);
		void readResponse1(ServiceTransactionClientRead::SPtr serviceTransactionRead);
		void writeRequestError(uint32_t channelId, const std::string& clientHandle, OpcUaStackCore::OpcUaStatusCode statusCode);
		void writeResponse(ServiceTransactionClientWrite::SPtr serviceTransactionWrite);
		void monitorStartRequestError(uint32_t channelId, const std::string& clientHandle, OpcUaStackCore::OpcUaStatusCode statusCode);
		void monitorStopRequestError(uint32_t channelId, const std::string& clientHandle, OpcUaStackCore::OpcUaStatusCode statusCode);
		void timerLoop(void);
		void polling(void);

		bool polling_;
		OpcUaStackCore::Config* config_;
		OpcUaClientIf* opcUaClientIf_;
		OpcUaClientConfig::SPtr opcUaClientConfig_;

		boost::shared_ptr<boost::asio::io_service::strand> strand_ = nullptr;
		OpcUaStackCore::MessageBus::SPtr messageBus_ = nullptr;
		OpcUaStackCore::IOThread::SPtr ioThread_ = nullptr;

		State state_;
		typedef std::map<uint32_t, uint32_t> NamespaceMap;
		NamespaceMap namespaceMap_;

		OpcUaStackClient::ServiceSetManager serviceSetManager_;
		OpcUaStackClient::SessionService::SPtr sessionService_;
		OpcUaStackClient::AttributeService::SPtr attributeService_;
		OpcUaStackClient::SubscriptionService::SPtr subscriptionService_;
		OpcUaStackClient::MonitoredItemService::SPtr monitoredItemService_;

		uint32_t reconnectTimeout_;
		uint32_t subscriptionId_;

		ValueInfo valueInfo_;
		MonitorManager monitorManager_;

		OpcUaStackCore::SlotTimerElement::SPtr slotTimerElement_;
	};

}

#endif
