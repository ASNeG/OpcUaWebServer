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

#include "OpcUaWebServer/Messages/Message.h"

namespace OpcUaWebServer
{

	Message::Message(MessageType messageType)
	: messageType_(messageType)
	, channelId_(0)
	, clientHandle_("")
	, statusCode_("")
	, body_()
	{
	}

	Message::~Message(void)
	{
	}

	Message::MessageType
	Message::string2MessageType(const std::string& messageType)
	{
		if (messageType == "READ_REQUEST") return MT_ReadRequest;
		else if (messageType == "READ_RESPONSE") return MT_ReadResponse;
		else if (messageType == "WRITE_REQUEST") return MT_WriteRequest;
		else if (messageType == "WRITE_RESPONSE") return MT_WriteResponse;
		else if (messageType == "VALUELIST_REQUEST") return MT_ValueListRequest;
		else if (messageType == "VALUELIST_RESPONSE") return MT_ValueListResponse;
		else if (messageType == "VALUEINFO_REQUEST") return MT_ValueInfoRequest;
		else if (messageType == "VALUEINFO_RESPONSE") return  MT_ValueInfoResponse;
		else if (messageType == "MONITORSTART_REQUEST") return MT_MonitorStartRequest;
		else if (messageType == "MONITORSTART_RESPONSE") return MT_MonitorStartResponse;
		else if (messageType == "MONITORSTOP_REQUEST") return MT_MonitorStopRequest;
		else if (messageType == "MONITORSTOP_RESPONSE") return MT_MonitorStopResponse;
		else if (messageType == "MONITORUPDATE_MESSAGE") return MT_MonitorUpdateMessage;
		else if (messageType == "HISTORICALREAD_REQUEST") return MT_HistorcialReadRequest;
		else if (messageType == "HISTORICALREAD_RESPONSE") return MT_HistoricalReadResponse;
		else if (messageType == "CHANNELCLOSE_MESSAGE") return MT_ChannelCloseMessage;
		else if (messageType == "ERROR") return MT_Error;
		else return MT_Unknown;
	}

	std::string
	Message::messageTypeString(void)
	{
		switch (messageType_)
		{
			case MT_ReadRequest: return "READ_REQUEST";
			case MT_ReadResponse: return "READ_RESPONSE";
			case MT_WriteRequest: return "WRITE_REQUEST";
			case MT_WriteResponse: return "WRITE_RESPONSE";
			case MT_ValueListRequest: return "VALUELIST_REQUEST";
			case MT_ValueListResponse: return "VALUELIST_RESPONSE";
			case MT_ValueInfoRequest: return "VALUEINFO_REQUEST";
			case MT_ValueInfoResponse: return "VALUEINFO_RESPONSE";
			case MT_MonitorStartRequest: return "MONITORSTART_REQUEST";
			case MT_MonitorStartResponse: return "MONITORSTART_RESPONSE";
			case MT_MonitorStopRequest: return "MONITORSTOP_REQUEST";
			case MT_MonitorStopResponse: return "MONITORSTOP_RESPONSE";
			case MT_MonitorUpdateMessage: return "MONITORUPDATE_MESSAGE";
			case MT_HistorcialReadRequest: return "HISTORICALREAD_REQUEST";
			case MT_HistoricalReadResponse: return "HISTORICALREAD_RESPONSE";
			case MT_ChannelCloseMessage: return "CHANNELCLOSE_MESSAGE";
			case MT_Error: return "ERROR";
			default: return "UNKNOWN";
		}
	}

	Message::MessageType
	Message::messageType(void)
	{
		return messageType_;
	}

	void
	Message::channelId(uint32_t channelId)
	{
		channelId_ = channelId;
	}

	uint32_t
	Message::channelId(void)
	{
		return channelId_;
	}

	void
	Message::clientHandle(const std::string& clientHandle)
	{
		clientHandle_ = clientHandle;
	}

	std::string&
	Message::clientHandle(void)
	{
		return clientHandle_;
	}

	void
	Message::statusCode(const std::string& statusCode)
	{
		statusCode_ = statusCode;
	}

	std::string&
	Message::statusCode(void)
	{
		return statusCode_;
	}

	void
	Message::body(boost::property_tree::ptree& body)
	{
		body_ = body;
	}

	boost::property_tree::ptree&
	Message::body(void)
	{
		return body_;
	}

}
