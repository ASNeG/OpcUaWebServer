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

#ifndef __OpcUaWebServer_ClientManager_h__
#define __OpcUaWebServer_ClientManager_h__

#include  <boost/asio/strand.hpp>
#include "OpcUaStackCore/Utility/IOThread.h"
#include "OpcUaStackCore/Certificate/CryptoManager.h"
#include "OpcUaWebServer/WebSocket/WebSocketMessage.h"
#include "OpcUaWebServer/WebGateway/RequestHeader.h"
#include "OpcUaWebServer/WebGateway/NotifyHeader.h"
#include "OpcUaWebServer/WebGateway/Client.h"

namespace OpcUaWebServer
{

	class ClientManager
	{
	  public:
		using SendMessageCallback = std::function<void (WebSocketMessage::SPtr& webSocketMessage)>;
		using DisconnectChannelCallback = std::function<void (uint32_t channelId)>;
		using ShutdownCallback = std::function<void (void)>;

		ClientManager(void);
		virtual ~ClientManager(void);

		bool startup(
			OpcUaStackCore::IOThread::SPtr& ioThread,
			OpcUaStackCore::MessageBus::SPtr& messageBus,
			OpcUaStackCore::CryptoManager::SPtr& cryptoManager
		);
		bool shutdown(void);

		void sendMessageCallback(const SendMessageCallback& sendMessageCallback);
		void disconnectChannelCallback(const DisconnectChannelCallback& disconnectChannelCallback);
		void receiveMessage(WebSocketMessage::SPtr& webSocketMessage);

	  private:
		void handleChannelClose(
			uint32_t channelId,
			RequestHeader requestHeader,
			boost::property_tree::ptree& body
		);
		void handleLogin(
			uint32_t channelId,
			RequestHeader requestHeader,
			boost::property_tree::ptree& requestBody
		);
		void handleLogout(
			uint32_t channelId,
			RequestHeader requestHeader,
			boost::property_tree::ptree& requestBody
		);
		void handleRequest(
			uint32_t channelId,
			RequestHeader requestHeader,
			boost::property_tree::ptree& requestBody
		);
		void sendResponse(
			uint32_t channelId,
			RequestHeader& requestHeader,
			boost::property_tree::ptree& responseBody
		);
		void sendNotify(
			uint32_t channelId,
			NotifyHeader& notifyHeader,
			boost::property_tree::ptree& notifyBody
		);
		void sendErrorResponse(
			uint32_t channelId,
			RequestHeader& requestHeader,
			OpcUaStackCore::OpcUaStatusCode statusCode
		);

		boost::mutex mutex_;
		Client::Map clientMap_;
		std::multimap<uint32_t, std::string> channelIdSessionIdMap_;


		DisconnectChannelCallback disconnectChannelCallback_;
		SendMessageCallback sendMessageCallback_;
		ShutdownCallback shutdownCallback_;

		OpcUaStackCore::IOThread::SPtr ioThread_ = nullptr;
		OpcUaStackCore::MessageBus::SPtr messageBus_ = nullptr;
		OpcUaStackCore::CryptoManager::SPtr cryptoManager_ = nullptr;
	};

}

#endif
