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
#include "OpcUaWebServer/WS/WSServerBase.h"

using namespace OpcUaStackCore;

namespace OpcUaWebServer
{


	WSServerBase::WSServerBase(void)
	: ioThread_()
	, wsConfig_()
	, state_(State::Down)
	, mutex_()
	, acceptor_()
	, wsConnectionMap_()
	{
	}

	WSServerBase::~WSServerBase(void)
	{
	}

	bool
	WSServerBase::startup(
		const WSConfig& wsConfig,
		IOThread::SPtr& ioThread
	)
	{
		ioThread_ = ioThread;
		wsConfig_ = wsConfig;

		// check state
		if (state_ != State::Down) {
			return false;
		}

		// bind web socket to endpoint
		bind();

	    // accept new connection from endpoint
		accept();

		state_ = State::Up;
		return true;
	}

	void
	WSServerBase::shutdown(
		ShutdownCompleteCallback& shutdownCompleteCallback
	)
	{
		shutdownCompleteCallback_ = shutdownCompleteCallback;

		// check state
		if (state_ == State::Down) {
			if (shutdownCompleteCallback_) {
				shutdownCompleteCallback_();
			}
			return;
		}
		state_ = State::Shutdown;

		// close acceptor socket
	    if(acceptor_) {
	    	boost::system::error_code ec;
	        acceptor_->close(ec);
	    }
	}

	bool
	WSServerBase::bind(void)
	{
		auto endpoint = boost::asio::ip::tcp::endpoint(
			boost::asio::ip::address::from_string(wsConfig_.address()),
			wsConfig_.port()
		);
		acceptor_ = std::unique_ptr<boost::asio::ip::tcp::acceptor>(
			new boost::asio::ip::tcp::acceptor(ioThread_->ioService()->io_service())
		);
	    acceptor_->open(endpoint.protocol());
	    acceptor_->set_option(boost::asio::socket_base::reuse_address(true));
	    acceptor_->bind(endpoint);
	    acceptor_->listen();

	    Log(Info, "open web socket")
	        .parameter("Address", wsConfig_.address())
			.parameter("Port", wsConfig_.port());

	    return true;
	}

	bool
	WSServerBase::accept(void)
	{
		// create new web socket connection
		auto connection = std::make_shared<WSConnection>(ioThread_);

		// accept new
		acceptor_->async_accept(
			*connection->socket(),
			[this, connection](const boost::system::error_code &ec) {

				// accept new connection if web socket server is not stopped
				if(ec != boost::asio::error::operation_aborted) {
				    accept();
				}

				// handle handshake to open connection
		        if(!ec) {

		        	// applications that require lower latency on every packet
		        	// sent should be run on sockets with TCP_NODELAY enabled
			        boost::asio::ip::tcp::no_delay option(true);
			        connection->socket()->set_option(option);

			        insertConnection(connection);

			        //read_handshake(connection);
			        return;
			    }

		        // handle error
		        if (state_ == State::Shutdown) {
		        	state_ = State::Down;
					if (shutdownCompleteCallback_) {
						shutdownCompleteCallback_();
					}
		        }
			}
		);
		return true;
	}

	void
	WSServerBase::insertConnection(const WSConnection::SPtr& connection)
	{
		boost::mutex::scoped_lock g(mutex_);
		wsConnectionMap_.insert(
			std::make_pair(connection->id(), connection)
		);
	}

	void
	WSServerBase::deleteConnection(const WSConnection::SPtr& connection)
	{
		boost::mutex::scoped_lock g(mutex_);
		wsConnectionMap_.erase(connection->id());
	}

}
