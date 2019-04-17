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
#include "OpcUaWebServer/OpcUaClient/ValueListResponseData.h"

using namespace OpcUaStackCore;

namespace OpcUaWebServer
{

	ValueListResponseData::ValueListResponseData(void)
	: valueVec_()
	{
	}

	ValueListResponseData::~ValueListResponseData(void)
	{
	}

	void
	ValueListResponseData::valueVec(std::vector<std::string>& valueVec)
	{
		valueVec_ = valueVec;
	}

	std::vector<std::string>&
	ValueListResponseData::valueVec(void)
	{
		return valueVec_;
	}

	bool
	ValueListResponseData::jsonDecode(boost::property_tree::ptree& pt)
	{
		return false;
	}

	bool
	ValueListResponseData::jsonEncode(boost::property_tree::ptree& pt)
	{
		boost::property_tree::ptree variables;

		for (auto it = valueVec_.begin(); it != valueVec_.end(); it++) {
			boost::property_tree::ptree arrayElement;
			OpcUaString str(*it);
			if (!str.jsonEncode(arrayElement)) return false;
			variables.push_back(std::make_pair("", arrayElement));
		}

		pt.add_child("Variables", variables);
		return true;
	}

}


