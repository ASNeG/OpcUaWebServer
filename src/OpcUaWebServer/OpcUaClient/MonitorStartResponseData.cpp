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

#include "OpcUaStackCore/Base/Log.h"
#include "OpcUaStackCore/BuildInTypes/OpcUaString.h"
#include "OpcUaWebServer/OpcUaClient/MonitorStartResponseData.h"

using namespace OpcUaStackCore;

namespace OpcUaWebServer
{

	MonitorStartResponseData::MonitorStartResponseData(void)
	: statusCode_("")
	{
	}

	MonitorStartResponseData::~MonitorStartResponseData(void)
	{
	}

	void
	MonitorStartResponseData::statusCode(const std::string& statusCode)
	{
		statusCode_ = statusCode;
	}

	std::string&
	MonitorStartResponseData::statusCode(void)
	{
		return statusCode_;
	}

	bool
	MonitorStartResponseData::jsonEncode(boost::property_tree::ptree& pt)
	{
		if (statusCode_.length() == 0) return true;

		boost::property_tree::ptree statusCode;
		OpcUaString statusCodeString = statusCode_;
		if (!statusCodeString.jsonEncode(statusCode)) return false;
		pt.add_child("StatusCode", statusCode);
		return true;
	}

	bool
	MonitorStartResponseData::jsonDecode(boost::property_tree::ptree& pt)
	{
		return false;
	}

}


