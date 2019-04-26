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

#ifndef __OpcUaWebServer_WSConnection_h__
#define __OpcUaWebServer_WSConnection_h__

#include <boost/asio.hpp>
#include <string>
#include "OpcUaStackCore/Utility/IOThread.h"
#include "OpcUaWebServer/WS/Id.h"

using namespace OpcUaStackCore;

namespace OpcUaWebServer
{

	class WSConnectionCallbacks
	{
	  public:
	};

	class WSConnection
	: public std::enable_shared_from_this<WSConnection>
	, public Id
	{
	  public:
		typedef std::shared_ptr<WSConnection> SPtr;

		WSConnection(IOThread::SPtr& ioThread);
		virtual ~WSConnection(void);

		std::unique_ptr<boost::asio::ip::tcp::socket>& socket(void);

		void disconnect(void);

		void readHandshake(void);
		void writeHandshake(void);

	  private:
		IOThread::SPtr ioThread_;
		std::unique_ptr<boost::asio::ip::tcp::socket> socket_;
		boost::asio::ip::tcp::endpoint remoteEndpoint_;

		boost::asio::streambuf readBuffer_;
	};

}

#endif
