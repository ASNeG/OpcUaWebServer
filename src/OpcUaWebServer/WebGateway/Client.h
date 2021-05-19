/*
   Copyright 2015-2021 Kai Huebl (kai@huebl-sgh.de)

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

#ifndef __OpcUaWebServer_Client_h__
#define __OpcUaWebServer_Client_h__

#include <boost/shared_ptr.hpp>
#include <map>
#include "OpcUaStackCore/Utility/IOThread.h"
#include "OpcUaStackCore/Certificate/CryptoManager.h"
#include "OpcUaStackCore/StandardDataTypes/EventFieldList.h"
#include "OpcUaStackCore/MessageBus/MessageBus.h"
#include "OpcUaStackClient/ServiceSet/ServiceSetManager.h"
#include "OpcUaWebServer/OpcUaClient/RequestInfo.h"

using namespace OpcUaStackCore;
using namespace OpcUaStackClient;

namespace OpcUaWebServer
{

	class Client
	{
	  public:
		typedef boost::shared_ptr<Client> SPtr;
		typedef std::map<std::string, Client::SPtr> Map;

		typedef std::function<void (const std::string& sessionStatus)> SessionStatusCallback;
		typedef std::function<void (uint32_t subscriptionId, const std::string& subscriptionStatus)> SubscriptionStatusCallback;
		typedef std::function<void (uint32_t clientHandle, const OpcUaDataValue& dataValue)> DataChangeCallback;
		typedef std::function<void (uint32_t clientHandle, const OpcUaVariantArray& event)> EventCallback;
		typedef std::function<void (OpcUaStatusCode statusCode, boost::property_tree::ptree& responseBody)> LogoutResponseCallback;
		typedef std::function<void (OpcUaStatusCode statusCode, boost::property_tree::ptree& responseBody)> MessageResponseCallback;

		class LogoutContext
		{
		  public:
			using SPtr = boost::shared_ptr<LogoutContext>;

			boost::property_tree::ptree requestBody_;
			LogoutResponseCallback logoutResponseCallback_;
		};

		Client(void);
		virtual ~Client(void);

		std::string id(void);
		void cryptoManager(OpcUaStackCore::CryptoManager::SPtr& cryptoManager);
	    void ioThread(OpcUaStackCore::IOThread::SPtr& ioThread);
	    void messageBus(OpcUaStackCore::MessageBus::SPtr& messageBus);

		//
		// session service functions
		//
		OpcUaStatusCode login(
			boost::property_tree::ptree& requestBoy,
			boost::property_tree::ptree& responseBody,
			const SessionStatusCallback& sessionStatusCallback
		);
		void logout(
			boost::property_tree::ptree& requestBody,
			const LogoutResponseCallback& logoutResponseCallback
		);

		//
		// attribute service functions
		//
		void read(
			const RequestInfo& requestInfo,
			boost::property_tree::ptree& requestBody,
			const MessageResponseCallback& messageResponseCallback
		);
		void write(
			const RequestInfo& requestInfo,
			boost::property_tree::ptree& requestBody,
			const MessageResponseCallback& messageResponseCallback
		);
		void historyRead(
			const RequestInfo& requestInfo,
			boost::property_tree::ptree& requestBody,
			const MessageResponseCallback& messageResponseCallback
		);

		//
		// view service functions
		//
		void browse(
			const RequestInfo& requestInfo,
			boost::property_tree::ptree& requestBody,
			const MessageResponseCallback& messageResponseCallback
		);
		void browseNext(
			const RequestInfo& requestInfo,
			boost::property_tree::ptree& requestBody,
			const MessageResponseCallback& messageResponseCallback
		);
		void translateBrowsePathsToNodeIds(
			const RequestInfo& requestInfo,
			boost::property_tree::ptree& requestBody,
			const MessageResponseCallback& messageResponseCallback
		);

		//
		// method service
		//
		void call(
			const RequestInfo& requestInfo,
			boost::property_tree::ptree& requestBody,
			const MessageResponseCallback& messageResponseCallback
		);

		//
		// subscription service
		//
		void subscriptionStatusCallback(
		    const SubscriptionStatusCallback& subscriptionStatusCallback
		);
		void dataChangeCallback(
			const DataChangeCallback dataChangeCallback
		);
		void eventCallback(
			const EventCallback eventCallback
		);
		void createSubscription(
			const RequestInfo& requestInfo,
			boost::property_tree::ptree& requestBody,
			const MessageResponseCallback& messageResponseCallback
		);
		void deleteSubscriptions(
			const RequestInfo& requestInfo,
			boost::property_tree::ptree& requestBody,
			const MessageResponseCallback& messageResponseCallback
		);

		//
		// monitored item service
		//
		void createMonitoredItems(
			const RequestInfo& requestInfo,
			boost::property_tree::ptree& requestBody,
			const MessageResponseCallback& messageResponseCallback
		);
		void deleteMonitoredItems(
			const RequestInfo& requestInfo,
			boost::property_tree::ptree& requestBody,
			const MessageResponseCallback& messageResponseCallback
		);

	  private:
		void logoutComplete(void);
		bool initViewService(const MessageResponseCallback& messageResponseCallback);
		bool initAttributeService(const MessageResponseCallback& messageResponseCallback);
		bool initMethodService(const MessageResponseCallback& messageResponseCallback);
		bool initSubscriptionService(const MessageResponseCallback& messageResponseCallback);
		bool initMonitoredItemService(const MessageResponseCallback& messageResponseCallback);

		static uint32_t gId_;
		uint32_t id_;

		LogoutContext::SPtr logoutContext_ = nullptr;
		SessionServiceStateId sessionState_ = SessionServiceStateId::Disconnected;

		boost::shared_ptr<boost::asio::io_service::strand> strand_ = nullptr;
		OpcUaStackCore::MessageBus::SPtr messageBus_ = nullptr;
		OpcUaStackCore::IOThread::SPtr ioThread_ = nullptr;

		SessionStatusCallback sessionStatusCallback_;
		SubscriptionStatusCallback subscriptionStatusCallback_;
		DataChangeCallback dataChangeCallback_;
		EventCallback eventCallback_;
		LogoutResponseCallback logoutResponseCallback_; // FIXME: todo - muss weg

		std::string sessionName_;

		CryptoManager::SPtr cryptoManager_;
		OpcUaStackClient::ServiceSetManager serviceSetManager_;
		OpcUaStackClient::SessionService::SPtr sessionService_;
		OpcUaStackClient::AttributeService::SPtr attributeService_;
		OpcUaStackClient::ViewService::SPtr viewService_;
		OpcUaStackClient::MethodService::SPtr methodService_;
		OpcUaStackClient::SubscriptionService::SPtr subscriptionService_;
		OpcUaStackClient::MonitoredItemService::SPtr monitoredItemService_;
	};

}

#endif
