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

#include <boost/make_shared.hpp>
#include "OpcUaStackCore/Base/Log.h"
#include "OpcUaWebServer/WebSocket/WebSocketServer.h"
#include "OpcUaWebServer/WebSocket/SocketWS.h"

namespace OpcUaWebServer
{

	WebSocketServer::WebSocketServer(WebSocketConfig* webSocketConfig)
	: WebSocketServerBase(webSocketConfig)
	, webSocketConfig_(webSocketConfig)
	{
		if (webSocketConfig_->strand().get() == nullptr) {
			webSocketConfig_->strand(webSocketConfig_->ioThread()->createStrand());
		}
	}

	WebSocketServer::~WebSocketServer(void)
	{
	}

	void
	WebSocketServer::startup(const StartupCompleteCallback& startupCompleteCallback)
	{
		webSocketConfig_->strand()->dispatch(
			[this, startupCompleteCallback](){ startupStrand(startupCompleteCallback); }
		);
	}

	void
	WebSocketServer::startupStrand(const StartupCompleteCallback& startupCompleteCallback)
	{
		Log(Info, "open websocket listener socket")
			.parameter("Address", webSocketConfig_->address())
			.parameter("Port", webSocketConfig_->port());

		tcpAcceptor_.listen();
		accept();

		startupCompleteCallback(true);
	}

	void
	WebSocketServer::shutdown(const ShutdownCompleteCallback& shutdownCompleteCallback)
	{
		ShutdownCompleteCallback tmpShutdownCompleteCallback = shutdownCompleteCallback;
		webSocketConfig_->strand()->dispatch(
			[this, tmpShutdownCompleteCallback](){ shutdownStrand(tmpShutdownCompleteCallback); }
		);
	}

	void
	WebSocketServer::shutdownStrand(const ShutdownCompleteCallback& shutdownCompleteCallback)
	{
		shutdownCompleteCallback(true);
	}

	void
	WebSocketServer::sendMessage(
	    WebSocketMessage::SPtr& webSocketMessage,
		const SendCompleteCallback& sendCompleteCallback
	)
	{
		webSocketConfig_->strand()->dispatch(
			[this, webSocketMessage, sendCompleteCallback](void) mutable {
			    sendMessageStrand(webSocketMessage, sendCompleteCallback);
		    }
		);
	}

	void
	WebSocketServer::sendMessageStrand(
	    WebSocketMessage::SPtr& webSocketMessage,
		const SendCompleteCallback& sendCompleteCallback
	)
	{
		WebSocketServerBase::sendMessage(
			webSocketMessage,
			sendCompleteCallback
		);
	}

	WebSocketChannel*
	WebSocketServer::createWebSocketChannel(void)
	{
		SocketIf::SPtr socketIf = boost::make_shared<SocketWS>(webSocketConfig_->ioThread()->ioService()->io_service());
		auto webSocketChannel = new WebSocketChannel(socketIf);

		return webSocketChannel;
	}

	void
	WebSocketServer::accept(void)
	{
		auto webSocketChannel = createWebSocketChannel();

		webSocketChannel->socket().async_accept(
			webSocketConfig_->strand(),
			&tcpAcceptor_,
			[this, webSocketChannel](const boost::system::error_code& error) {
				handleAccept(error, webSocketChannel);
			}
		);
	}

	void
	WebSocketServer::handleAccept(const boost::system::error_code& error, WebSocketChannel* webSocketChannel)
	{
		if (error) {
			Log(Info, "handle accept error")
				.parameter("Address", webSocketConfig_->address())
				.parameter("Port", webSocketConfig_->port());

			delete webSocketChannel;
			return;
		}

		webSocketChannel->partner_ = webSocketChannel->socket().remote_endpoint();
		Log(Debug, "WebSocketServer accept connection from client")
			.parameter("Address", webSocketChannel->partner_.address().to_string())
			.parameter("Port", webSocketChannel->partner_.port())
			.parameter("Connections", webSocketChannelMap_.size());

		initWebSocketChannel(webSocketChannel);
		performHandshake(webSocketChannel);

		if (!active_) {
			return;
		}

		accept();
	}

	void
	WebSocketServer::addWebSocketChannel(uint32_t count)
	{
		if (webSocketConfig_->maxConnections() == 0) {
			return;
		}

		if (count >= webSocketConfig_->maxConnections() && active_) {
			Log(Warning, "close websocket listener socket, because max connection limit reached")
				.parameter("Address", webSocketConfig_->address())
				.parameter("Port", webSocketConfig_->port())
				.parameter("MaxConnections", count);
			active_ = false;
		}
	}

	void
	WebSocketServer::delWebSocketChannel(uint32_t count)
	{
		if (webSocketConfig_->maxConnections() == 0) {
			return;
		}

		if (count < webSocketConfig_->maxConnections() && !active_) {
			Log(Info, "open websocket listener socket")
				.parameter("Address", webSocketConfig_->address())
				.parameter("Port", webSocketConfig_->port())
				.parameter("MaxConnections", count);

			active_ = true;
			accept();
		}
	}

}
