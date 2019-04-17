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
#include "OpcUaWebServer/OpcUaClient/ValueInfoRequestData.h"

using namespace OpcUaStackCore;

namespace OpcUaWebServer
{

	ValueInfoRequestData::ValueInfoRequestData(void)
	: variableVec_()
	{
	}

	ValueInfoRequestData::~ValueInfoRequestData(void)
	{
	}

	void
	ValueInfoRequestData::variableVec(std::vector<std::string>& variableVec)
	{
		variableVec_ = variableVec;
	}

	std::vector<std::string>&
	ValueInfoRequestData::variableVec(void)
	{
		return variableVec_;
	}

	bool
	ValueInfoRequestData::jsonDecode(boost::property_tree::ptree& pt)
	{
		boost::optional<boost::property_tree::ptree&> variables;
		variables = pt.get_child_optional("Variables");
		if (!variables) return false;

		boost::property_tree::ptree::iterator it;
		for (it = variables->begin(); it != variables->end(); it++) {
			boost::property_tree::ptree arrayElement = it->second;

			OpcUaString element;
			if (!element.jsonDecode(arrayElement)) return false;
			variableVec_.push_back(element.value());
		}

		return true;
	}

	bool
	ValueInfoRequestData::jsonEncode(boost::property_tree::ptree& pt)
	{
		return false;
	}

}


