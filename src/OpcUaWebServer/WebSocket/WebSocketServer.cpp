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

#include "OpcUaStackCore/Base/Log.h"
#include "OpcUaWebServer/WebSocket/WebSocketServer.h"

namespace OpcUaWebServer
{

	WebSocketServer::WebSocketServer(WebSocketConfig* webSocketConfig)
	: WebSocketServerBase(webSocketConfig)
	, webSocketConfig_(webSocketConfig)
	{
	}

	WebSocketServer::~WebSocketServer(void)
	{
	}

	bool
	WebSocketServer::startup(void)
	{
		Log(Info, "open websocket listener socket")
			.parameter("Address", webSocketConfig_->address())
			.parameter("Port", webSocketConfig_->port());

		tcpAcceptor_.listen();
		accept();

		return true;
	}

	bool
	WebSocketServer::shutdown(void)
	{
		return true;
	}

	void
	WebSocketServer::accept(void)
	{
		WebSocketChannel* webSocketChannel = new WebSocketChannel(webSocketConfig_->ioThread()->ioService()->io_service());
		tcpAcceptor_.async_accept(
			webSocketChannel->socket(),
			boost::bind(&WebSocketServer::handleAccept, this, boost::asio::placeholders::error, webSocketChannel)
		);
	}

	void
	WebSocketServer::handleAccept(const boost::system::error_code& error, WebSocketChannel* webSocketChannel)
	{
		if (error) {
			delete webSocketChannel;
			return;
		}

		webSocketChannel->partner_ = webSocketChannel->socket().remote_endpoint();
		Log(Debug, "WebSocketServer accept connection from client")
			.parameter("Address", webSocketChannel->partner_.address().to_string())
			.parameter("Port", webSocketChannel->partner_.port());

		initWebSocketChannel(webSocketChannel);
		receiveHandshake(webSocketChannel);

		accept();
	}

}
