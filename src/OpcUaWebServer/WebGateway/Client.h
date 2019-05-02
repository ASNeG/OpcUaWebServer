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

		Client(void);
		virtual ~Client(void);

		uint32_t id(void);
		void ioThread(IOThread::SPtr& ioThread);
		void cryptoManager(CryptoManager::SPtr& cryptoManager);

		OpcUaStatusCode login(
			boost::property_tree::ptree& requestBoy,
			boost::property_tree::ptree& responseBody
		);

	  private:
		static uint32_t gId_;
		uint32_t id_;

		IOThread::SPtr ioThread_;
		CryptoManager::SPtr cryptoManager_;
		ServiceSetManager serviceSetManager_;
		SessionService::SPtr sessionService_;
	};

}

#endif
