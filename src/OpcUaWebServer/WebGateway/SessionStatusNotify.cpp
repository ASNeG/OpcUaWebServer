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
#include "OpcUaWebServer/WebGateway/SessionStatusNotify.h"

using namespace OpcUaStackCore;

namespace OpcUaWebServer
{

	SessionStatusNotify::SessionStatusNotify(void)
	: sessionStatus_("")
	{
	}

	SessionStatusNotify::~SessionStatusNotify(void)
	{
	}

	std::string&
	SessionStatusNotify::sessionStatus(void)
	{
		return sessionStatus_;
	}

	bool
	SessionStatusNotify::jsonEncode(boost::property_tree::ptree& pt)
	{
		pt.put("SessionStatus", sessionStatus_);
		return true;
	}

	bool
	SessionStatusNotify::jsonDecode(boost::property_tree::ptree& pt)
	{
		// get session status from json message
		boost::optional<std::string> sessionStatus = pt.get_optional<std::string>("SessionStatus");
		if (!sessionStatus) {
			Log(Error, "message body do not contain session status");
			return false;
		}
		sessionStatus_ = *sessionStatus;

		return true;
	}

}

