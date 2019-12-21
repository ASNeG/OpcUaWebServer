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

#include "OpcUaWebServer/WebSocket/SocketWSS.h"

namespace OpcUaWebServer
{

	SocketWSS::SocketWSS(
		boost::asio::io_service& io_service,
		boost::asio::ssl::context& context
	)
	: SocketIf()
	, stream_(io_service, context)
	{
	}

	SocketWSS::~SocketWSS(void)
	{
	}

	boost::asio::ip::tcp::endpoint
	SocketWSS::remote_endpoint(
		void
	)
	{
		return stream_.next_layer().remote_endpoint();
	}

	boost::asio::ip::tcp::endpoint
	SocketWSS::local_endpoint(
		void
	)
	{
		return stream_.next_layer().local_endpoint();
	}

	void
	SocketWSS::close(
		void
	)
	{
		stream_.next_layer().close();
	}

	void
	SocketWSS::cancel(
		void
	)
	{
		stream_.next_layer().cancel();
	}

	void
	SocketWSS::performHandshake(
		boost::shared_ptr<boost::asio::io_service::strand>& strand,
		const PerformHandshakeCompleteCallback& performHandshakeCompleteCallback
	)
	{
		strand_ = strand;
		performHandshakeCompleteCallback_ = performHandshakeCompleteCallback;

		boost::asio::ip::tcp::no_delay option(true);
		stream_.lowest_layer().set_option(option);

		stream_.async_handshake(
			boost::asio::ssl::stream_base::server,
			[this, performHandshakeCompleteCallback](const boost::system::error_code& error) {
				strand_->dispatch(
				    [this, error](void) {
						performHandshakeCompleteCallback_(error);
					}
				);
			}
		);
	}

	void
	SocketWSS::async_read_until(
		boost::shared_ptr<boost::asio::io_service::strand>& strand,
		boost::asio::streambuf& recvBuffer,
		const std::string& content,
		const ReceiveCallback& receiveCallback
	)
	{
		strand_ = strand;
		receiveCallback_ = receiveCallback;

		boost::asio::async_read_until(
			stream_.next_layer(),
			recvBuffer,
			content,
			[this](const boost::system::error_code& error, std::size_t bytes_transfered) {
			    strand_->dispatch(
				    [this, error, bytes_transfered](void) {
					    receiveCallback_(error, bytes_transfered);
					}
			    );
			}
		);
	}

	void
	SocketWSS::async_read_exactly(
		boost::shared_ptr<boost::asio::io_service::strand>& strand,
		boost::asio::streambuf& recvBuffer,
		size_t contentSize,
		const ReceiveCallback& receiveCallback
	)
	{
		strand_ = strand;
		receiveCallback_ = receiveCallback;

		boost::asio::async_read(
			stream_.next_layer(),
			recvBuffer,
			boost::asio::transfer_exactly(contentSize),
			[this](const boost::system::error_code& error, std::size_t bytes_transfered) {
			    strand_->dispatch(
				    [this, error, bytes_transfered](void) {
					    receiveCallback_(error, bytes_transfered);
					}
			    );
			}
		);
	}

	void
	SocketWSS::async_write(
		boost::shared_ptr<boost::asio::io_service::strand>& strand,
		boost::asio::streambuf& sendBuffer,
		const WriteCompleteCallback& writeCompleteCallback
	)
	{
		  strand_ = strand;
		  writeCompleteCallback_  = writeCompleteCallback;

		  boost::asio::async_write(
			  stream_.next_layer(),
			  sendBuffer,
			  [this](const boost::system::error_code& error, std::size_t bytes_transfered) {
			      strand_->dispatch(
				      [this, error, bytes_transfered](void) {
					      writeCompleteCallback_(error, bytes_transfered);
					  }
			      );
			  }
		  );
	}

	void
	SocketWSS::async_accept(
		boost::shared_ptr<boost::asio::io_service::strand>& strand,
		OpcUaStackCore::TCPAcceptor* acceptor,
		const AcceptCallback& acceptCallback
	)
	{
		acceptor->async_accept(
			stream_.next_layer(),
			strand,
			acceptCallback
		);
	}

}
