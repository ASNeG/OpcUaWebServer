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

   Autor: Kai Huebl (kai@huebl-sgh.de), Aleksey Timin (atimin@gmail.com)

*/

#ifndef __OpcUaWebServer_OpcUaClientManager_h__
#define __OpcUaWebServer_OpcUaClientManager_h__

#include "OpcUaStackCore/Base/Config.h"
#include "OpcUaWebServer/Messages/Message.h"
#include "OpcUaWebServer/OpcUaClient/OpcUaClientManagerIf.h"
#include "OpcUaWebServer/OpcUaClient/OpcUaClientConfig.h"
#include "OpcUaWebServer/OpcUaClient/OpcUaClient.h"

namespace OpcUaWebServer
{

	class OpcUaClientManager
	: public OpcUaClientIf
	{
	  public:
		OpcUaClientManager(void);
		~OpcUaClientManager(void);

		bool startup(
			OpcUaStackCore::Config* config,
			OpcUaClientManagerIf* opcUaClientManagerIf,
			OpcUaStackCore::IOThread::SPtr ioThread,
			OpcUaStackCore::MessageBus::SPtr& messageBus,
			OpcUaStackCore::CryptoManager::SPtr& cryptoManager
		);
		bool shutdown(void);

		void receiveMessage(Message::SPtr& requestMessage);

		//- OpcUaClientIf -----------------------------------------------------
		void clientMessage(Message::SPtr& message);
		//- OpcUaclientIf -----------------------------------------------------

	  private:
		void receiveReadMessage(Message::SPtr& requestMessage);
		void receiveWriteMessage(Message::SPtr& requestMessage);
		void receiveValueListMessage(Message::SPtr& requestMessage);
		void receiveValueInfoMessage(Message::SPtr& requestMessage);
		void receiveMonitorStartMessage(Message::SPtr& requestMessage);
		void receiveMonitorStopMessage(Message::SPtr& requestMessage);
		void receiveHistoricalReadMessage(Message::SPtr& requestMessage);
		void receiveChannelCloseMessage(Message::SPtr& requestMessage);
		bool readClientConfig(void);
		bool startupClient(void);
		void sendErrorMessage(Message::SPtr &requestMessage, Message messageType,
				OpcUaStackCore::OpcUaStatusCode statusCode) const;

		bool enable_;

		OpcUaStackCore::Config* config_ = nullptr;
		OpcUaStackCore::IOThread::SPtr ioThread_ = nullptr;
		OpcUaStackCore::MessageBus::SPtr messageBus_ = nullptr;
		OpcUaStackCore::CryptoManager::SPtr cryptoManager_ = nullptr;

		OpcUaClientManagerIf* opcUaClientManagerIf_;
		OpcUaClientConfig::Map opcUaClientConfigMap_;
		OpcUaClient::Map opcUaClientMap_;
		ValueInfoEntry::Map valueInfoEntryMap_;

	};

}

#endif
