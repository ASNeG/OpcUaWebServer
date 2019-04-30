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

#include <boost/algorithm/string/replace.hpp>
#include <OpcUaWebServer/WebGateway/RequestHeader.h>
#include "OpcUaStackCore/Base/Log.h"

using namespace OpcUaStackCore;

namespace OpcUaWebServer
{

	RequestHeader::RequestHeader(void)
	: messageType_("")
	, clientHandle_("")
	{
	}

	RequestHeader::RequestHeader(const RequestHeader& RequestHeader, bool response)
	: messageType_(RequestHeader.messageType_)
	, clientHandle_(RequestHeader.clientHandle_)
	{
		if (response) {
			boost::replace_all(messageType_, "Request", "Response");
		}
	}

	RequestHeader::~RequestHeader(void)
	{
	}

	std::string&
	RequestHeader::messageType(void)
	{
		return messageType_;
	}

	std::string&
	RequestHeader::clientHandle(void)
	{
		return clientHandle_;
	}

	bool
	RequestHeader::jsonEncode(boost::property_tree::ptree& pt)
	{
		boost::property_tree::ptree headerElement;
		headerElement.put("MessageType", messageType_);
		headerElement.put("ClientHandle", clientHandle_);
		pt.push_back(std::make_pair("Header", headerElement));

		return true;
	}

	bool
	RequestHeader::jsonDecode(boost::property_tree::ptree& pt)
	{
		// get message type from json message
		boost::optional<std::string> messageType = pt.get<std::string>("Header.MessageType");
		if (!messageType) {
			Log(Error, "message header do not contain message type");
			return false;
		}
		messageType_ = *messageType;

		// get client handle from json message
		boost::optional<std::string> clientHandle = pt.get<std::string>("Header.ClientHandle");
		if (!clientHandle) {
			Log(Error, "message header do not contain client handle");
			return false;
		}
		clientHandle_ = *clientHandle;

		return true;
	}

}

