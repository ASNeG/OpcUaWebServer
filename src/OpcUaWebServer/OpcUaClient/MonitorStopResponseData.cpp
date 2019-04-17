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
#include "OpcUaWebServer/OpcUaClient/MonitorStopResponseData.h"

using namespace OpcUaStackCore;

namespace OpcUaWebServer
{

	MonitorStopResponseData::MonitorStopResponseData(void)
	: statusCode_("")
	{
	}

	MonitorStopResponseData::~MonitorStopResponseData(void)
	{
	}

	void
	MonitorStopResponseData::statusCode(const std::string& statusCode)
	{
		statusCode_ = statusCode;
	}

	std::string&
	MonitorStopResponseData::statusCode(void)
	{
		return statusCode_;
	}

	bool
	MonitorStopResponseData::jsonEncode(boost::property_tree::ptree& pt)
	{
		if (statusCode_.length() == 0) return true;

		boost::property_tree::ptree statusCode;
		OpcUaString statusCodeString = statusCode_;
		if (!statusCodeString.jsonEncode(statusCode)) return false;
		pt.add_child("StatusCode", statusCode);
		return true;
	}

	bool
	MonitorStopResponseData::jsonDecode(boost::property_tree::ptree& pt)
	{
		return false;
	}

}


