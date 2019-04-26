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

#ifndef __OpcUaWebServer_WSServerBase_h__
#define __OpcUaWebServer_WSServerBase_h__

#include <boost/asio.hpp>
#include "OpcUaStackCore/Utility/IOThread.h"
#include "OpcUaWebServer/WS/WSConfig.h"
#include "OpcUaWebServer/WS/WSConnection.h"

using namespace OpcUaStackCore;

namespace OpcUaWebServer
{

	class WSServerBase
	{
	  public:
		enum class State {
			Up,
			Shutdown,
			Down
		};
		typedef std::function<void (void)> ShutdownCompleteCallback;
		typedef std::map<std::string, WSConnection::SPtr> WSConnectionMap;

		WSServerBase(void);
		~WSServerBase(void);

		bool startup(
			const WSConfig& wsConfig,
			IOThread::SPtr& ioThread
		);
		void shutdown(
			ShutdownCompleteCallback& shutdownCompleteCallback
		);

	  private:
		bool bind(void);
		bool accept(void);
		void insertConnection(const WSConnection::SPtr& connection);
		void deleteConnection(const WSConnection::SPtr& connection);

		State state_;
		boost::mutex mutex_;
		IOThread::SPtr ioThread_;
		WSConfig wsConfig_;

		ShutdownCompleteCallback shutdownCompleteCallback_;
		std::unique_ptr<boost::asio::ip::tcp::acceptor> acceptor_;
		WSConnectionMap wsConnectionMap_;
	};

}

#endif
