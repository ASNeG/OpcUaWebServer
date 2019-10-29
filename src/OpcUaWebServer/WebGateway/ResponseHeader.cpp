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
		auto str = messageType().toStdString();
		boost::replace_all(str, "Request", "Response");
		messageType() = OpcUaString(str);
	}

	ResponseHeader::ResponseHeader(
	    const OpcUaString& messageType,
		const OpcUaString& clientHandle,
		const OpcUaString& sessionId
	)
	: RequestHeader(messageType, clientHandle, sessionId, 0)
	, statusCode_(Success)
	{
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
	ResponseHeader::jsonEncodeImpl(boost::property_tree::ptree& pt) const
    {
    	bool rc = true;
    	rc = rc & RequestHeader::jsonEncodeImpl(pt);
    	rc = rc & jsonNumberEncode(pt, (uint32_t)statusCode_, "StatusCode");
    	return rc;
    }

    bool
	ResponseHeader::jsonDecodeImpl(const boost::property_tree::ptree& pt)
    {
       	bool rc = true;
        rc = rc & RequestHeader::jsonDecodeImpl(pt);
        rc = rc & jsonNumberDecode(pt, *(uint32_t*)&statusCode_, "StatusCode", true, (uint32_t)0);
        return rc;
    }

}

