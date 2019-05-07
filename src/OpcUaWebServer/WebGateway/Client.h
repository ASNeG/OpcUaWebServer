/*
   Copyright 2015 Kai Huebl (kai@huebl-sgh.de)

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
#include "OpcUaStackClient/ServiceSet/ServiceSetManager.h"

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
		typedef std::function<void (OpcUaStatusCode statusCode, boost::property_tree::ptree& responseBody)> LogoutResponseCallback;
		typedef std::function<void (OpcUaStatusCode statusCode, boost::property_tree::ptree& responseBody)> MessageResponseCallback;

		Client(void);
		virtual ~Client(void);

		std::string id(void);
		void ioThread(IOThread::SPtr& ioThread);
		void cryptoManager(CryptoManager::SPtr& cryptoManager);

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
			boost::property_tree::ptree& requestBody,
			const MessageResponseCallback& messageResponseCallback
		);
		void write(
			boost::property_tree::ptree& requestBody,
			const MessageResponseCallback& messageResponseCallback
		);
		void historyRead(
			boost::property_tree::ptree& requestBody,
			const MessageResponseCallback& messageResponseCallback
		);

		//
		// method service
		//
		void call(
			boost::property_tree::ptree& requestBody,
			const MessageResponseCallback& messageResponseCallback
		);

		//
		// subscription service
		//
		void createSubscription(
			boost::property_tree::ptree& requestBody,
			const MessageResponseCallback& messageResponseCallback
		);
		void deleteSubscriptions(
			boost::property_tree::ptree& requestBody,
			const MessageResponseCallback& messageResponseCallback
		);

		//
		// monitored item service
		//
		void createMonitoredItems(
			boost::property_tree::ptree& requestBody,
			const MessageResponseCallback& messageResponseCallback
		);
		void deleteMonitoredItems(
			boost::property_tree::ptree& requestBody,
			const MessageResponseCallback& messageResponseCallback
		);

	  private:
		bool initAttributeService(const MessageResponseCallback& messageResponseCallback);
		bool initMethodService(const MessageResponseCallback& messageResponseCallback);
		bool initSubscriptionService(const MessageResponseCallback& messageResponseCallback);
		bool initMonitoredItemService(const MessageResponseCallback& messageResponseCallback);

		static uint32_t gId_;
		uint32_t id_;

		SessionStatusCallback sessionStatusCallback_;
		LogoutResponseCallback logoutResponseCallback_;

		IOThread::SPtr ioThread_;
		CryptoManager::SPtr cryptoManager_;
		ServiceSetManager serviceSetManager_;
		SessionService::SPtr sessionService_;
		AttributeService::SPtr attributeService_;
		MethodService::SPtr methodService_;
		SubscriptionService::SPtr subscriptionService_;
		MonitoredItemService::SPtr monitoredItemService_;
	};

}

#endif
