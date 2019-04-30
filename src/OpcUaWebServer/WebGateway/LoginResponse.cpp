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

#include <OpcUaWebServer/WebGateway/LoginResponse.h>
#include "OpcUaStackCore/Base/Log.h"

using namespace OpcUaStackCore;

namespace OpcUaWebServer
{

	LoginResponse::LoginResponse(void)
	: sessionId_("")
	{
	}

	LoginResponse::~LoginResponse(void)
	{
	}


	bool
	LoginResponse::jsonEncode(boost::property_tree::ptree& pt)
	{
		pt.put("SessionId", sessionId_);
		return true;
	}

	bool
	LoginResponse::jsonDecode(boost::property_tree::ptree& pt)
	{
		// get session id from json message
		boost::optional<std::string> sessionId = pt.get_optional<std::string>("SessionId");
		if (!sessionId) {
			Log(Error, "message body do not contain session id");
			return false;
		}
		sessionId_ = *sessionId;

		return true;
	}

}

