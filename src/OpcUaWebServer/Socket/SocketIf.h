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

#ifndef __OpcUaWebServer_SocketIf_h__
#define __OpcUaWebServer_SocketIf_h__

#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include "OpcUaStackCore/Network/TCPAcceptor.h"

namespace OpcUaWebServer
{

	class SocketIf
	{
	  public:
		typedef boost::shared_ptr<SocketIf> SPtr;

		using PerformHandshakeCompleteCallback = std::function<void(const boost::system::error_code& error)>;
		using ReceiveCallback = std::function<void(const boost::system::error_code& error, std::size_t bytes_transfered)>;
		using WriteCompleteCallback = std::function<void(const boost::system::error_code& error,std::size_t bytes_transfered)>;
		using AcceptCallback = std::function<void(const boost::system::error_code& error)>;

		SocketIf(void) {}
		virtual ~SocketIf(void) {}

		virtual boost::asio::ip::tcp::endpoint remote_endpoint(
			void
		) = 0;
		virtual boost::asio::ip::tcp::endpoint local_endpoint(
			void
		) = 0;
		virtual void close(
			void
		) = 0;
		virtual void cancel(
			void
		) = 0;
		virtual void performHandshake(
			boost::shared_ptr<boost::asio::io_service::strand>& strand,
			const PerformHandshakeCompleteCallback& performHandshakeCompleteCallback
		) = 0;
		virtual void async_read_until(
			boost::shared_ptr<boost::asio::io_service::strand>& strand,
			boost::asio::streambuf& recvBuffer,
			const std::string& content,
			const ReceiveCallback& receiveCallback
		) = 0;
		virtual void async_read_exactly(
			boost::shared_ptr<boost::asio::io_service::strand>& strand,
			boost::asio::streambuf& recvBuffer,
			size_t contentSize,
			const ReceiveCallback& receiveCallback
		) = 0;
		virtual void async_write(
			boost::shared_ptr<boost::asio::io_service::strand>& strand,
			boost::asio::streambuf& sendBuffer,
			const WriteCompleteCallback& writeCompleteCallback
		) = 0;
		virtual void async_accept(
			boost::shared_ptr<boost::asio::io_service::strand>& strand,
			OpcUaStackCore::TCPAcceptor* acceptor,
			const AcceptCallback& acceptCallback
		) = 0;
	};

}

#endif
