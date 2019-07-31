/*
   Copyright 2015-2019 Kai Huebl (kai@huebl-sgh.de)

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

#ifndef __OpcUaWebServer_WebSocket_h__
#define __OpcUaWebServer_WebSocket_h__

#include "OpcUaStackCore/Base/IOService.h"
#include "OpcUaStackCore/Base/Config.h"
#include "OpcUaWebServer/WebSocket/WebSocketServer.h"
#include "OpcUaWebServer/WebSocket/WebSocketConfig.h"

using namespace OpcUaStackCore;

namespace OpcUaWebServer
{

	class WebSocket
	{
	  public:
		typedef std::function<void (bool error)> StartupCompleteCallback;
		typedef std::function<void (bool error)> ShutdownCompleteCallback;
		typedef std::function<void (bool error)> SendCompleteCallback;

		WebSocket(void);
		~WebSocket(void);

		void startup(
			Config* config,
			const IOThread::SPtr& ioThread,
			const WebSocketServer::ReceiveMessageCallback& receiveMessageCallback,
			const StartupCompleteCallback& startupCompleteCallback
		);
		bool shutdown(
			void
		);

		bool sendMessage(
		    WebSocketMessage& webSocketMessage
		);

	  private:
		void startupStrand(
			Config* config,
			const IOThread::SPtr& ioThread,
			const WebSocketServer::ReceiveMessageCallback& receiveMessageCallback,
			const StartupCompleteCallback& startupCompleteCallback
		);

		bool getWebSocketConfig(Config* config);

		boost::shared_ptr<boost::asio::strand> strand_ = nullptr;
		StartupCompleteCallback startupCompleteCallback_;
		ShutdownCompleteCallback shutdownCompleteCallback_;

		WebSocketServer::SPtr webSocketServer_ = nullptr;
		WebSocketConfig webSocketConfig_;
	};

}

#endif
