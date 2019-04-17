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

#include "OpcUaWebServer/OpcUaClient/PtreeConverter.h"

namespace OpcUaWebServer
{

	PtreeConverter::PtreeConverter(void)
	{
	}

	PtreeConverter::~PtreeConverter(void)
	{
	}

	bool
	PtreeConverter::xml2Json(boost::property_tree::ptree& xml, boost::property_tree::ptree& json)
	{
		// single variable
		if (xml.size() == 0) {
			boost::optional<std::string> variable = xml.get_value_optional<std::string>();
			if (variable) json.put_value(variable);
			return true;
		}

		// hash or array variable
		boost::property_tree::ptree::iterator it;
		for (it = xml.begin(); it != xml.end(); it++) {
			boost::optional<std::string> type = it->second.get_optional<std::string>("<xmlattr>.type");
			if (type && *type == "array") {
				// array value
				boost::property_tree::ptree newJson;
				if (!xml2JsonArray(it->second, newJson)) return false;
				json.add_child(it->first, newJson);
			}

			else {
				// hash value
				boost::property_tree::ptree newJson;
				if (!xml2Json(it->second, newJson)) return false;
				json.add_child(it->first, newJson);
			}
		}

		return true;
	}

	bool
	PtreeConverter::xml2JsonArray(boost::property_tree::ptree& xml, boost::property_tree::ptree& json)
	{
		boost::property_tree::ptree::iterator it;
		for (it = xml.begin(); it != xml.end(); it++) {
			if (it->first == "<xmlattr>") continue;

			boost::property_tree::ptree arrayElement;
			if (it->first == "value") {
				// simple value
				boost::optional<std::string> value = it->second.get_value_optional<std::string>();
				if (value) {
					arrayElement.put_value(*value);
				}
			}

			else {
				// object value
				boost::property_tree::ptree objectElement;
				if (!xml2JsonArray(it->second, objectElement)) return false;
				arrayElement.add_child(it->first, objectElement);
			}

			json.push_back(std::make_pair("", arrayElement));
		}
		return true;
	}

}
