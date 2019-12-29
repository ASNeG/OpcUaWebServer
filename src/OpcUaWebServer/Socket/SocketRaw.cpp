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

#include <boost/system/error_code.hpp>
#include <OpcUaWebServer/Socket/SocketRaw.h>

namespace OpcUaWebServer
{

	SocketRaw::SocketRaw(boost::asio::io_service& io_service)
	: SocketIf()
	, tcpConnection_(io_service)
	{
	}

	SocketRaw::~SocketRaw(void)
	{
	}

	boost::asio::ip::tcp::endpoint
	SocketRaw::remote_endpoint(
			void
	)
	{
		return tcpConnection_.socket().remote_endpoint();
	}

	boost::asio::ip::tcp::endpoint
	SocketRaw::local_endpoint(
		void
	)
	{
		return tcpConnection_.socket().local_endpoint();
	}

	void
	SocketRaw::close(
		void
	)
	{
		tcpConnection_.close();
	}

	void
	SocketRaw::cancel(
		void
	)
	{
		tcpConnection_.cancel();
	}

	void
	SocketRaw::performHandshake(
		boost::shared_ptr<boost::asio::io_service::strand>& strand,
		const PerformHandshakeCompleteCallback& performHandshakeCompleteCallback
	)
	{
		performHandshakeCompleteCallback(boost::system::errc::make_error_code(boost::system::errc::success));
	}

	void
	SocketRaw::async_read_until(
		boost::shared_ptr<boost::asio::io_service::strand>& strand,
		boost::asio::streambuf& recvBuffer,
		const std::string& content,
		const ReceiveCallback& receiveCallback
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
	SocketRaw::async_read_exactly(
		boost::shared_ptr<boost::asio::io_service::strand>& strand,
		boost::asio::streambuf& recvBuffer,
		size_t contentSize,
		const ReceiveCallback& receiveCallback
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
	SocketRaw::async_write(
		boost::shared_ptr<boost::asio::io_service::strand>& strand,
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

	void
	SocketRaw::async_accept(
		boost::shared_ptr<boost::asio::io_service::strand>& strand,
		OpcUaStackCore::TCPAcceptor* acceptor,
		const AcceptCallback& acceptCallback
	)
	{
		acceptor->async_accept(
			tcpConnection_.socket(),
			strand,
			acceptCallback
		);
	}

}
