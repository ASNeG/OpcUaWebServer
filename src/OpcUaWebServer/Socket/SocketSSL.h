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

#ifndef __OpcUaWebServer_SocketSSL_h__
#define __OpcUaWebServer_SocketSSL_h__

#include <boost/asio/ssl.hpp>
#include "OpcUaWebServer/Socket/SocketIf.h"

namespace OpcUaWebServer
{

	class SocketSSL
	: public SocketIf
	{
	  public:
		typedef boost::shared_ptr<SocketSSL> SPtr;

		SocketSSL(boost::asio::io_service& io_service, boost::asio::ssl::context& context);
		virtual ~SocketSSL(void);

		boost::asio::ip::tcp::endpoint remote_endpoint(
			void
		) override;
		boost::asio::ip::tcp::endpoint local_endpoint(
			void
		) override;
		void close(
			void
		) override;
		void cancel(
			void
		) override;
		void performHandshake(
			boost::shared_ptr<boost::asio::io_service::strand>& strand,
			const PerformHandshakeCompleteCallback& performHandshakeCompleteCallback
		) override;
		void async_read_until(
			boost::shared_ptr<boost::asio::io_service::strand>& strand,
			boost::asio::streambuf& recvBuffer,
			const std::string& content,
			const ReceiveCallback& receiveCallback
		) override;
		void async_read_exactly(
			boost::shared_ptr<boost::asio::io_service::strand>& strand,
			boost::asio::streambuf& recvBuffer,
			size_t contentSize,
			const ReceiveCallback& receiveCallback
		) override;
		void async_write(
			boost::shared_ptr<boost::asio::io_service::strand>& strand,
			boost::asio::streambuf& sendBuffer,
			const WriteCompleteCallback& writeCompleteCallback
		) override;
		void async_accept(
			boost::shared_ptr<boost::asio::io_service::strand>& strand,
			OpcUaStackCore::TCPAcceptor* acceptor,
			const AcceptCallback& acceptCallback
		) override;

	  private:
		boost::asio::ssl::stream<boost::asio::ip::tcp::socket> stream_;
		boost::shared_ptr<boost::asio::io_service::strand> strand_ = nullptr;
		PerformHandshakeCompleteCallback performHandshakeCompleteCallback_ = nullptr;
		AcceptCallback acceptCallback_ = nullptr;
		ReceiveCallback receiveCallback_ = nullptr;
		WriteCompleteCallback writeCompleteCallback_ = nullptr;
	};

}

#endif
