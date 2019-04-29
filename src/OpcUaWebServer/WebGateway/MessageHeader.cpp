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
#include "OpcUaWebServer/WebGateway/MessageHeader.h"

using namespace OpcUaStackCore;

namespace OpcUaWebServer
{

	MessageHeader::MessageHeader(void)
	: messageType_("")
	, clientHandle_("")
	{
	}

	MessageHeader::~MessageHeader(void)
	{
	}

	std::string&
	MessageHeader::messageType(void)
	{
		return messageType_;
	}

	std::string&
	MessageHeader::clientHandle(void)
	{
		return clientHandle_;
	}

	bool
	MessageHeader::jsonEncode(boost::property_tree::ptree& pt)
	{
		boost::property_tree::ptree headerElement;
		headerElement.put("MessageType", messageType_);
		headerElement.put("ClientHandle", clientHandle_);
		pt.push_back(std::make_pair("Header", headerElement));

		return true;
	}

	bool
	MessageHeader::jsonDecode(boost::property_tree::ptree& pt)
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

