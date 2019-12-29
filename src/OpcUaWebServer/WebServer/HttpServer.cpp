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
#include "OpcUaStackCore/Utility/Environment.h"
#include "OpcUaWebServer/Socket/SocketRaw.h"
#include "OpcUaWebServer/Socket/SocketSSL.h"
#include "OpcUaWebServer/WebServer/HttpServer.h"

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
		Log(Info, "open http listener socket")
			.parameter("Address", httpConfig_->address())
			.parameter("Port", httpConfig_->port());

		tcpAcceptor_.listen(128);

		// set ssl configuration parameter if exist
		ssl_ = false;
		auto csrFile = Environment::confDir() + std::string("/ssl/crt/websocket.crt");
		auto keyFile = Environment::confDir() + std::string("/ssl/key/websocket.key");

		if (ssl_) {
			Log(Info, "use https protocol")
		    	.parameter("CsrFile", csrFile)
				.parameter("KeyFile", keyFile);
		}
		else {
			Log(Info, "use http protocol");
		}

		if (ssl_) {
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
		tcpAcceptor_.close();
		shutdownCompleteCallback(true);
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
		if (error) {
			Log(Error, "HttpServer error");
			delete httpChannel;
			return;
		}

		httpChannel->partner_ = httpChannel->socket().remote_endpoint();
		Log(Debug, "HttpServer accept connection from client")
			.parameter("Address", httpChannel->partner_.address().to_string())
			.parameter("Port", httpChannel->partner_.port())
			.parameter("ChannelId", httpChannel->channelId_);

		ipLogger_.logout(httpChannel->partner_.address().to_string());

		receiveRequest(httpChannel);

		accept();
	}

}
