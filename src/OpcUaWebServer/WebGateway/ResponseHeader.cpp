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
#include <OpcUaWebServer/WebGateway/ResponseHeader.h>
#include "OpcUaStackCore/Base/Log.h"

using namespace OpcUaStackCore;

namespace OpcUaWebServer
{

	ResponseHeader::ResponseHeader(void)
	: RequestHeader()
	, statusCode_(Success)
	{
	}

	ResponseHeader::ResponseHeader(const RequestHeader& requestHeader)
	: RequestHeader(requestHeader)
	, statusCode_(Success)
	{
		boost::replace_all(messageType(), "Request", "Response");
	}

	ResponseHeader::~ResponseHeader(void)
	{
	}

	OpcUaStatusCode&
	ResponseHeader::statusCode(void)
	{
		return statusCode_;
	}

	bool
	ResponseHeader::jsonEncode(boost::property_tree::ptree& pt)
	{
		if (!RequestHeader::jsonEncode(pt)) return false;
		pt.put("Header.StatusCode", OpcUaStatusCodeMap::shortString(statusCode_));
		return true;
	}

	bool
	ResponseHeader::jsonDecode(boost::property_tree::ptree& pt)
	{
		if (!RequestHeader::jsonDecode(pt)) return false;

		// get status code from json message
		boost::optional<std::string> statusCode = pt.get_optional<std::string>("Header.StatusCode");
		if (!statusCode) {
			statusCode_ = Success;
			return true;
		}
		statusCode_ = OpcUaStatusCodeMap::statusCode(*statusCode);

		return true;
	}

}

