/*
   Copyright 2015 Kai Huebl (kai@huebl-sgh.de)

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

#include "OpcUaWebServer/WebSocket/WebSocketChannel.h"

namespace OpcUaWebServer
{

	uint32_t WebSocketChannel::gChannelId_ = 0;

	WebSocketChannel::WebSocketChannel(boost::asio::io_service& io_service)
	: TCPConnection(io_service)
	, recvBuffer_()
	, sendBuffer_()
	, webSocketRequest_()
	, webSocketResponse_()
	, slotTimerElement_()
	, timeout_(false)
	{
		slotTimerElement_ = constructSPtr<SlotTimerElement>();

		gChannelId_++;
		std::stringstream ss;
		id_ = gChannelId_;
		ss << "WebSocket" << gChannelId_;
		channelId_ = ss.str();
	}

	WebSocketChannel::~WebSocketChannel(void)
	{
	}

}
