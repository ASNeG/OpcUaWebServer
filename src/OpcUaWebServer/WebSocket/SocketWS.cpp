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

#include "OpcUaWebServer/WebSocket/SocketWS.h"

namespace OpcUaWebServer
{

	SocketWS::SocketWS(boost::asio::io_service& io_service)
	: SocketIf()
	, tcpConnection_(io_service)
	{
	}

	SocketWS::~SocketWS(void)
	{
	}

	boost::asio::ip::tcp::endpoint
	SocketWS::remote_endpoint(
			void
	)
	{
		return tcpConnection_.socket().remote_endpoint();
	}

	void
	SocketWS::close(
		void
	)
	{
		return tcpConnection_.close();
	}

	void
	SocketWS::cancel(
		void
	)
	{
		return tcpConnection_.cancel();
	}

	void
	SocketWS::async_read_until(
		boost::shared_ptr<boost::asio::strand>& strand,
		boost::asio::streambuf& recvBuffer,
		const std::string& content,
		ReceiveCallback& receiveCallback
	)
	{
		  tcpConnection_.async_read_until(
			  strand,
			  recvBuffer,
			  receiveCallback,
			  content
		  );
	}

	void
	SocketWS::async_read_exactly(
		boost::shared_ptr<boost::asio::strand>& strand,
		boost::asio::streambuf& recvBuffer,
		size_t contentSize,
		ReceiveCallback& receiveCallback
	)
	{
		  tcpConnection_.async_read_exactly(
			  strand,
			  recvBuffer,
			  receiveCallback,
			  contentSize
		  );
	}

	void
	SocketWS::async_write(
		boost::shared_ptr<boost::asio::strand>& strand,
		boost::asio::streambuf& sendBuffer,
		const WriteCompleteCallback& writeCompleteCallback
	)
	{
		tcpConnection_.async_write(
			strand,
			sendBuffer,
			writeCompleteCallback
		);
	}

}
