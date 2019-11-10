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

namespace OpcUaWebServer
{

	class SocketIf
	{
	  public:
		typedef boost::shared_ptr<SocketIf> SPtr;

		using AcceptCallback = std::function<void(const boost::system::error_code& error)>;
		using ReceiveCallback = std::function<void(const boost::system::error_code& error, std::size_t bytes_transfered)>;
		using WriteCompleteCallback = std::function<void(const boost::system::error_code& error,std::size_t bytes_transfered)>;

		SocketIf(void) {}
		virtual ~SocketIf(void) {}

		virtual boost::asio::ip::tcp::endpoint remote_endpoint(
			void
		) = 0;
		virtual void close(
			void
		) = 0;
		virtual void cancel(
			void
		) = 0;
		virtual void async_read_until(
			boost::shared_ptr<boost::asio::strand>& strand,
			boost::asio::streambuf& recvBuffer,
			const std::string& content,
			ReceiveCallback& receiveCallback
		) = 0;
		virtual void async_read_exactly(
			boost::shared_ptr<boost::asio::strand>& strand,
			boost::asio::streambuf& recvBuffer,
			size_t contentSize,
			ReceiveCallback& receiveCallback
		) = 0;
		virtual void async_write(
			boost::shared_ptr<boost::asio::strand>& strand,
			boost::asio::streambuf& sendBuffer,
			const WriteCompleteCallback& writeCompleteCallback
		) = 0;
	};

}

#endif
