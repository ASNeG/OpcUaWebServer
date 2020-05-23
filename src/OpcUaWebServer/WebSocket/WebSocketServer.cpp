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

#include <boost/make_shared.hpp>
#include "OpcUaStackCore/Base/Log.h"
#include "OpcUaWebServer/Socket/SocketRaw.h"
#include "OpcUaWebServer/Socket/SocketSSL.h"
#include "OpcUaWebServer/WebSocket/WebSocketServer.h"

using namespace OpcUaStackCore;

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
		if (context_) {
			delete context_;
		}
	}

	std::string
	WebSocketServer::getPassword() const
	{
		return std::string("");
	}

	void
	WebSocketServer::startup(const StartupCompleteCallback& startupCompleteCallback)
	{
		webSocketConfig_->strand()->dispatch(
			[this, startupCompleteCallback](){
			    startupStrand(startupCompleteCallback);
		    }
		);
	}


	void
	WebSocketServer::startupStrand(const StartupCompleteCallback& startupCompleteCallback)
	{
		Log(Info, "open websocket listener socket")
			.parameter("Address", webSocketConfig_->address())
			.parameter("Port", webSocketConfig_->port());

		openWebSocketAcceptor();

		accept();

		startupCompleteCallback(true);
	}

	void
	WebSocketServer::shutdown(const ShutdownCompleteCallback& shutdownCompleteCallback)
	{
		ShutdownCompleteCallback tmpShutdownCompleteCallback = shutdownCompleteCallback;
		webSocketConfig_->strand()->dispatch(
			[this, tmpShutdownCompleteCallback](){
			    shutdownStrand(tmpShutdownCompleteCallback);
		    }
		);
	}

	void
	WebSocketServer::shutdownStrand(const ShutdownCompleteCallback& shutdownCompleteCallback)
	{
		shutdownFlag_ = true;
		shutdownCompleteCallback_ = shutdownCompleteCallback;

		// close listener socket
		if (active_) {
			closeWebSocketAcceptor();
		}

		// close channels
		for (auto pair : webSocketChannelMap_) {
			Log(Debug, "close websocket connection")
				.parameter("Id", pair.second->id_);
			pair.second->socket().close();
		}

		handleShutdown();
	}

	void
	WebSocketServer::handleShutdown(void)
	{
		Log(Debug, "handle shutdown")
			.parameter("Listener", active_ ? 1 : 0)
			.parameter("Connections", webSocketChannelMap_.size());

		// check if acceptor is active
		if (active_) {
			return;
		}

		// check if a socket connection is active
		if (!webSocketChannelMap_.empty()) {
			return;
		}

		// The shutdown process is ready.We can call the shutdown complete callback
		shutdownCompleteCallback_(true);
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
		SocketIf::SPtr socketIf;
		if (wss_) {
			socketIf = boost::make_shared<SocketSSL>(
				webSocketConfig_->ioThread()->ioService()->io_service(),
				*context_
			);
		}
		else {
			socketIf = boost::make_shared<SocketRaw>(
				webSocketConfig_->ioThread()->ioService()->io_service()
			);
		}
		return new WebSocketChannel(socketIf);
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
		if (error || shutdownFlag_) {
			Log(Info, "handle accept error")
				.parameter("Address", webSocketConfig_->address())
				.parameter("Port", webSocketConfig_->port());

			active_ = false;
			delete webSocketChannel;

			if (shutdownFlag_) {
				handleShutdown();
			}

			return;
		}

		webSocketChannel->partner_ = webSocketChannel->socket().remote_endpoint();
		Log(Debug, "WebSocketServer accept connection from client")
			.parameter("Address", webSocketChannel->partner_.address().to_string())
			.parameter("Port", webSocketChannel->partner_.port())
			.parameter("Connections", webSocketChannelMap_.size());

		// add a new web socket channel to the web socket channel map and call
		// function addWebSocketChannel
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
		// This function is called indirectly from function handleAccept (initWebSocketChannel)

		if (webSocketConfig_->maxConnections() == 0) {
			return;
		}

		if (count >= webSocketConfig_->maxConnections() && active_) {
			Log(Warning, "websocket max connection limit reached")
				.parameter("Address", webSocketConfig_->address())
				.parameter("Port", webSocketConfig_->port())
				.parameter("ActConnections", count);
			closeWebSocketAcceptor();
			active_ = false;
		}
	}

	void
	WebSocketServer::delWebSocketChannel(uint32_t count)
	{
		if (shutdownFlag_) {
			handleShutdown();
			return;
		}

		if (webSocketConfig_->maxConnections() == 0) {
			return;
		}

		if (count < webSocketConfig_->maxConnections() && !active_) {
			Log(Info, "websocket connection info")
				.parameter("Address", webSocketConfig_->address())
				.parameter("Port", webSocketConfig_->port())
				.parameter("ActConnections", count)
				.parameter("MaxConnections", webSocketConfig_->maxConnections());

			active_ = true;
			tcpAcceptor_.reopen();
			openWebSocketAcceptor();
			accept();
		}
	}

	void
	WebSocketServer::openWebSocketAcceptor(void)
	{
		Log(Info, "open websocket listener socket")
			.parameter("Address", webSocketConfig_->address())
			.parameter("Port", webSocketConfig_->port());

		tcpAcceptor_.listen();

		// set ssl configuration parameter if exist
		wss_ = webSocketConfig_->ssl();
		auto csrFile = webSocketConfig_->csrFile();
		auto keyFile = webSocketConfig_->keyFile();

		if (wss_) {
			Log(Info, "use wss protocol")
		    	.parameter("CsrFile", csrFile)
				.parameter("KeyFile", keyFile);

			// create context and add certificate and private key to context
			context_ = new boost::asio::ssl::context(
				boost::asio::ssl::context::sslv23
			);
			context_->set_options(
				boost::asio::ssl::context::default_workarounds |
				boost::asio::ssl::context::no_sslv2 |
				boost::asio::ssl::context::single_dh_use
			);
			context_->set_password_callback(boost::bind(&WebSocketServer::getPassword, this));
			context_->use_certificate_chain_file(csrFile);
			context_->use_private_key_file(keyFile, boost::asio::ssl::context::pem);
		}
		else {
			Log(Info, "use ws protocol");
		}
	}

	void
	WebSocketServer::closeWebSocketAcceptor(void)
	{
		Log(Debug, "close websocket listener socket")
			.parameter("Address", webSocketConfig_->address())
			.parameter("Port", webSocketConfig_->port());

		tcpAcceptor_.close();
		delete context_;
		context_ = nullptr;
	}

}
