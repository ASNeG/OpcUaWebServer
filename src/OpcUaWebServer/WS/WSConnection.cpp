/*
   Copyright 2019 Kai Huebl (kai@huebl-sgh.de)

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
#include "OpcUaWebServer/WS/WSConnection.h"

using namespace OpcUaStackCore;

namespace OpcUaWebServer
{

	WSConnection::WSConnection(IOThread::SPtr& ioThread)
	: ioThread_(ioThread)
	, socket_(new boost::asio::ip::tcp::socket(ioThread_->ioService()->io_service()))
	, Id("WSC")
	, remoteEndpoint_()
	, readBuffer_()
	{
	}

	WSConnection::~WSConnection(void)
	{
	}

	std::unique_ptr<boost::asio::ip::tcp::socket>&
	WSConnection::socket(void)
	{
		return socket_;
	}

	void
	WSConnection::disconnect(void)
	{
		// FIXME: todo
	}

	void
	WSConnection::readHandshake(void)
	{
		// get remote endpoint
		try {
			remoteEndpoint_ = socket_->lowest_layer().remote_endpoint();
	    }
	    catch(...) {
	    }
	    Log(Info, "accept web socket connection from client")
	        .parameter("Id", id())
	        .parameter("RemoteEndpoint", remoteEndpoint_);

	    // read handshake
	    boost::asio::async_read_until(
	    	*socket_,
			readBuffer_,
			"\r\n\r\n",
			[this](const boost::system::error_code &ec, std::size_t) {
	    		std::istream is(&readBuffer_);
	    		//if (RequestMessage::parse(is)) {
	    		//	writeHandshake();
	    		//}
	    	}
	    );
	}

	void
	WSConnection::writeHandshake(void)
	{
		// FIXME: todo
	}

}
