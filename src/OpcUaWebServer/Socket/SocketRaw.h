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

#ifndef __OpcUaWebServer_SocketRaw_h__
#define __OpcUaWebServer_SocketRaw_h__

#include "OpcUaStackCore/Network/TCPConnection.h"
#include "OpcUaWebServer/Socket/SocketIf.h"

namespace OpcUaWebServer
{

	class SocketRaw
	: public SocketIf
	{
	  public:
		typedef boost::shared_ptr<SocketRaw> SPtr;

		SocketRaw(boost::asio::io_service& io_service);
		virtual ~SocketRaw(void);

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
		OpcUaStackCore::TCPConnection tcpConnection_;
	};

}

#endif
