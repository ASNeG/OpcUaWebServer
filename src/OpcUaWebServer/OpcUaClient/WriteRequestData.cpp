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

#include <boost/make_shared.hpp>
#include "OpcUaStackCore/Base/Log.h"
#include "OpcUaWebServer/OpcUaClient/WriteRequestData.h"

using namespace OpcUaStackCore;

namespace OpcUaWebServer
{

	WriteRequestData::WriteRequestData(void)
	: variable_("")
	, dataValue_()
	, valueInfoEntry_()
	{
	}

	WriteRequestData::~WriteRequestData(void)
	{
	}

	void
	WriteRequestData::variable(const std::string& variable)
	{
		variable_ = variable;
	}

	std::string&
	WriteRequestData::variable(void)
	{
		return variable_;
	}

	void
	WriteRequestData::dataValue(OpcUaDataValue::SPtr dataValue)
	{
		dataValue_ = dataValue;
	}

	OpcUaDataValue::SPtr
	WriteRequestData::dataValue(void)
	{
		return dataValue_;
	}

	void
	WriteRequestData::valueInfoEntry(const ValueInfoEntry::SPtr& valueInfoEntry)
	{
		valueInfoEntry_ = valueInfoEntry;
	}

	ValueInfoEntry::SPtr&
	WriteRequestData::valueInfoEntry(void)
	{
		return valueInfoEntry_;
	}

	bool
	WriteRequestData::decodeVariable(boost::property_tree::ptree& pt)
	{
		// read value name
		boost::optional<std::string> variable = pt.get_optional<std::string>("Variable");
		if (!variable) {
			Log(Error, "Variable do not exist in write request");
			return false;
		}
		variable_ = *variable;
		return true;
	}

	bool
	WriteRequestData::jsonDecode(boost::property_tree::ptree& pt)
	{
		// read value
		//OpcUaBuildInType type = valueInfoEntry_->type_;
		//bool isArray = valueInfoEntry_->array_;

		try {
			dataValue_ = boost::make_shared<OpcUaDataValue>();
			if (!dataValue_->jsonDecode(pt, "Value")) {
				Log(Error, "data value error in write request");
				return false;
			}
		}
		catch (...) {
			return false;
		}

		return true;
	}

	bool
	WriteRequestData::jsonEncode(boost::property_tree::ptree& pt)
	{
		return false;
	}

}


