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

#include "OpcUaStackCore/Base/Log.h"
#include "OpcUaWebServer/OpcUaClient/ReadRequestData.h"

using namespace OpcUaStackCore;

namespace OpcUaWebServer
{

	ReadRequestData::ReadRequestData(void)
	: variable_("")
	{
	}

	ReadRequestData::~ReadRequestData(void)
	{
	}

	void
	ReadRequestData::variable(const std::string& variable)
	{
		variable_ = variable;
	}

	std::string&
	ReadRequestData::variable(void)
	{
		return variable_;
	}

	bool
	ReadRequestData::decode(boost::property_tree::ptree& pt)
	{
		boost::optional<std::string> variable = pt.get_optional<std::string>("Variable");
		if (!variable) {
			Log(Error, "Variable do not exist in read request");
			return false;
		}
		variable_ = *variable;
		return true;
	}

	bool
	ReadRequestData::encode(boost::property_tree::ptree& pt)
	{
		return false;
	}

}


