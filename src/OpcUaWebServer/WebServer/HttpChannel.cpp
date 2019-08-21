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

#include <boost/make_shared.hpp>
#include "OpcUaWebServer/WebServer/HttpChannel.h"

namespace OpcUaWebServer
{

	uint32_t HttpChannel::gChannelId_ = 0;

	HttpChannel::HttpChannel(boost::asio::io_service& io_service)
	: TCPConnection(io_service)
	, channelId_()
	, recvBuffer_()
	, sendBuffer_()
	, httpRequest_()
	, httpResponse_()
	, slotTimerElement_()
	, timeout_(false)
	{
		slotTimerElement_ = boost::make_shared<SlotTimerElement>();

		std::stringstream ss;
		ss << "Http" << gChannelId_++;
		channelId_ = ss.str();
	}

	HttpChannel::~HttpChannel(void)
	{
	}

}
