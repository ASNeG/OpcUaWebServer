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

#ifndef __OpcUaWebSocket_WebSocketServer_h__
#define __OpcUaWebSocket_WebSocketServer_h__

#include <boost/asio/ssl.hpp>
#include "OpcUaWebServer/WebSocket/WebSocketServerBase.h"

namespace OpcUaWebServer
{

	class WebSocketServer
	: public WebSocketServerBase
	, public OpcUaStackCore::Object
	{
	  public:
		typedef std::function<void (bool error)> StartupCompleteCallback;
		typedef std::function<void (bool error)> ShutdownCompleteCallback;
		typedef std::function<void (bool error)> SendCompleteCallback;
		typedef boost::shared_ptr<WebSocketServer> SPtr;

		WebSocketServer(WebSocketConfig* webSocketConfig);
		~WebSocketServer(void);

		void startup(
			const StartupCompleteCallback& startupCompleteCallback
		);
		void shutdown(
			const ShutdownCompleteCallback& shutdownCompleteCallback
		);
		void sendMessage(
		    WebSocketMessage::SPtr& webSocketMessage,
			const SendCompleteCallback& sendCompleteCallback
		);

		void addWebSocketChannel(uint32_t count) override;
		void delWebSocketChannel(uint32_t count) override;

	  private:
		std::string getPassword() const;
		void startupStrand(
			const StartupCompleteCallback& startupCompleteCallback
		);
		void shutdownStrand(
			const ShutdownCompleteCallback& shutdownCompleteCallback
		);
		void sendMessageStrand(
		    WebSocketMessage::SPtr& webSocketMessage,
			const SendCompleteCallback& sendCompleteCallback
		);
		void handleShutdown(void);
		void openWebSocketAcceptor(void);
		void closeWebSocketAcceptor(void);

		WebSocketChannel* createWebSocketChannel(void);
		void accept(void);
		void handleAccept(const boost::system::error_code& error, WebSocketChannel* webSocketChannel);

		bool wss_ = false;
		bool active_ = true;
		WebSocketConfig* webSocketConfig_ = nullptr;
		boost::asio::ssl::context* context_ = nullptr;

		bool shutdownFlag_ = false;
		ShutdownCompleteCallback shutdownCompleteCallback_;
	};

}

#endif
