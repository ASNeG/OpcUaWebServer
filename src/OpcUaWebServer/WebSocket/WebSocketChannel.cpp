/*
   Copyright 2015-2020 Kai Huebl (kai@huebl-sgh.de)

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
#include "OpcUaWebServer/WebSocket/WebSocketChannel.h"

using namespace OpcUaStackCore;

namespace OpcUaWebServer
{

	uint32_t WebSocketChannel::gChannelId_ = 0;

	WebSocketChannel::WebSocketChannel(SocketIf::SPtr& socketIf)
	: socketIf_(socketIf)
	, recvBuffer_()
	, sendBuffer_()
	, webSocketRequest_()
	, webSocketResponse_()
	, slotTimerElement_()
	, timeout_(false)
	{
		slotTimerElement_ = boost::make_shared<SlotTimerElement>();

		gChannelId_++;
		id_ = gChannelId_;
	}

	WebSocketChannel::~WebSocketChannel(void)
	{
	}

	std::string
	WebSocketChannel::getId(void)
	{
		std::stringstream ss;
		ss << id_;
		return ss.str();
	}

	SocketIf&
	WebSocketChannel::socket(void)
	{
		return *socketIf_.get();
	}

}
