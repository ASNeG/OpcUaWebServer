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
#include "OpcUaStackCore/Utility/Environment.h"
#include "OpcUaWebServer/Socket/SocketRaw.h"
#include "OpcUaWebServer/Socket/SocketSSL.h"
#include "OpcUaWebServer/WebServer/HttpServer.h"

using namespace OpcUaStackCore;

namespace OpcUaWebServer
{

	HttpServer::HttpServer(HttpConfig* httpConfig)
	: HttpServerBase(httpConfig)
	, httpConfig_(httpConfig)
	, ipLogger_()
	{
		ipLogger_.logFileName(httpConfig->logFileName());
		ipLogger_.maxLogFileNumber(httpConfig->maxLogFileNumber());
		ipLogger_.maxLogFileSize(httpConfig->maxLogFileSize());
		ipLogger_.maxIPAge(httpConfig->maxIPAge());
	}

	HttpServer::~HttpServer(void)
	{
	}

	void
	HttpServer::startup(
		const StartupCompleteCallback& startupCompleteCallback
	)
	{
		httpConfig_->strand()->dispatch(
			[this, startupCompleteCallback](){
			    startupStrand(startupCompleteCallback);
		    }
		);
	}

	void
	HttpServer::startupStrand(
		const StartupCompleteCallback& startupCompleteCallback
	)
	{
		openHttpAcceptor();

		accept();

		return startupCompleteCallback(true);
	}

	std::string
	HttpServer::getPassword() const
	{
		return std::string("");
	}

	void
	HttpServer::shutdown(
		const ShutdownCompleteCallback& shutdownCompleteCallback
	)
	{
		httpConfig_->strand()->dispatch(
			[this, shutdownCompleteCallback](){
			    shutdownStrand(shutdownCompleteCallback);
		    }
		);
	}

	void
	HttpServer::shutdownStrand(
		const ShutdownCompleteCallback& shutdownCompleteCallback
	)
	{
		shutdownFlag_ = true;
		shutdownCompleteCallback_ = shutdownCompleteCallback;

		// shutdown listener socket
		if (active_) {
			closeHttpAcceptor();
		}

		// close channels
		for (auto pair : httpChannelMap_) {
			Log(Debug, "close http connection")
				.parameter("Id", pair.second->id_);
			pair.second->socket().close();
		}

		handleShutdown();
	}

	void
	HttpServer::handleShutdown(void)
	{
		Log(Debug, "handle shutdown")
			.parameter("Listener", active_ ? 1 : 0)
			.parameter("Connections", httpChannelMap_.size());

		// check if acceptor is active
		if (active_) {
			return;
		}

		// check if a socket connection is active
		if (!httpChannelMap_.empty()) {
			return;
		}

		// The shutdown process is ready.We can call the shutdown complete callback
		shutdownCompleteCallback_(true);
	}

	HttpChannel*
	HttpServer::createHttpChannel(void)
	{
		SocketIf::SPtr socketIf;
		if (ssl_) {
			socketIf = boost::make_shared<SocketSSL>(
				httpConfig_->ioThread()->ioService()->io_service(),
				*context_
			);
		}
		else {
			socketIf = boost::make_shared<SocketRaw>(
				httpConfig_->ioThread()->ioService()->io_service()
			);
		}
		return new HttpChannel(socketIf);
	}

	void
	HttpServer::accept(void)
	{
		auto httpChannel = createHttpChannel();

		httpChannel->socket().async_accept(
			httpConfig_->strand(),
			&tcpAcceptor_,
			[this, httpChannel](const boost::system::error_code& error) {
				handleAccept(error, httpChannel);
			}
		);
	}

	void
	HttpServer::handleAccept(const boost::system::error_code& error, HttpChannel* httpChannel)
	{
		if (error || shutdownFlag_) {
			if (shutdownFlag_) {
				Log(Debug, "handle http server acceptor shutdown");
			}
			else {
				Log(Error, "handle http server acceptor error");
			}

			active_ = false;
			delete httpChannel;

			if (shutdownFlag_) {
				handleShutdown();
			}

			return;
		}

		httpChannel->partner_ = httpChannel->socket().remote_endpoint();
		Log(Debug, "HttpServer accept connection from client")
			.parameter("Address", httpChannel->partner_.address().to_string())
			.parameter("Port", httpChannel->partner_.port())
			.parameter("ChannelId", httpChannel->channelId_);

		ipLogger_.logout(httpChannel->partner_.address().to_string());

		initHttpChannel(httpChannel);
		performHandshake(httpChannel);

		if (!active_) {
			return;
		}

		accept();
	}

	void
	HttpServer::addHttpChannel(uint32_t count)
	{
		if (httpConfig_->maxConnections() == 0) {
			return;
		}

		if (count >= httpConfig_->maxConnections() && active_) {
			Log(Warning, "close http listener socket, because max connection limit reached")
				.parameter("Address", httpConfig_->address())
				.parameter("Port", httpConfig_->port())
				.parameter("MaxConnections", count);
			closeHttpAcceptor();
			active_ = false;
		}
	}

	void
	HttpServer::delHttpChannel(uint32_t count)
	{
		if (shutdownFlag_) {
			handleShutdown();
			return;
		}

		if (httpConfig_->maxConnections() == 0) {
			return;
		}

		if (count < httpConfig_->maxConnections() && !active_) {
			Log(Info, "open http listener socket")
				.parameter("Address", httpConfig_->address())
				.parameter("Port", httpConfig_->port())
				.parameter("MaxConnections", count);

			active_ = true;
			tcpAcceptor_.reopen();
			openHttpAcceptor();
			accept();
		}
	}

	void
	HttpServer::closeHttpChannel(void)
	{
		Log(Debug, "close http listener socket")
			.parameter("Address", httpConfig_->address())
			.parameter("Port", httpConfig_->port());

		tcpAcceptor_.close();
		delete context_;
		context_ = nullptr;
	}

	void
	HttpServer::openHttpAcceptor(void)
	{
		Log(Info, "open http listener socket")
			.parameter("Address", httpConfig_->address())
			.parameter("Port", httpConfig_->port());

		tcpAcceptor_.listen(128);

		// set ssl configuration parameter if exist
		ssl_ = httpConfig_->ssl();
		auto csrFile = Environment::confDir() + std::string("/ssl/crt/websocket.crt");
		auto keyFile = Environment::confDir() + std::string("/ssl/key/websocket.pem");

		if (ssl_) {
			Log(Info, "use https protocol")
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
			context_->set_password_callback(boost::bind(&HttpServer::getPassword, this));
			context_->use_certificate_chain_file(csrFile);
			context_->use_private_key_file(keyFile, boost::asio::ssl::context::pem);
		}
		else {
			Log(Info, "use http protocol");
		}
	}

    void
	HttpServer::closeHttpAcceptor(void)
    {
		Log(Debug, "close http listener socket")
			.parameter("Address", httpConfig_->address())
			.parameter("Port", httpConfig_->port());

		tcpAcceptor_.close();
		delete context_;
		context_ = nullptr;
    }

}
