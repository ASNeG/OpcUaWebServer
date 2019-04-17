/*
   Copyright 2015-2019 Kai Huebl (kai@huebl-sgh.de)

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

#ifndef __OpcUaWebServer_HttpChannel_h__
#define __OpcUaWebServer_HttpChannel_h__

#include "OpcUaStackCore/Network/TCPConnection.h"
#include "OpcUaStackCore/Utility/SlotTimer.h"
#include "OpcUaWebServer/WebServer/HttpRequest.h"
#include "OpcUaWebServer/WebServer/HttpResponse.h"

using namespace OpcUaStackCore;

namespace OpcUaWebServer
{

	class HttpChannel
	: public TCPConnection
	{
	  public:
		static uint32_t gChannelId_;

		HttpChannel(boost::asio::io_service& io_service);
		virtual ~HttpChannel(void);

		OpcUaStackCore::SlotTimerElement::SPtr slotTimerElement_;

		bool timeout_;
		std::string channelId_;
		boost::asio::streambuf recvBuffer_;
		boost::asio::streambuf sendBuffer_;
		HttpRequest httpRequest_;
		HttpResponse httpResponse_;
		boost::asio::ip::tcp::endpoint partner_;
	};

}

#endif
