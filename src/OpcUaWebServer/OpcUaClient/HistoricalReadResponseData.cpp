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
#include "OpcUaWebServer/OpcUaClient/HistoricalReadResponseData.h"

using namespace OpcUaStackCore;

namespace OpcUaWebServer
{

	HistoricalReadResponseData::HistoricalReadResponseData(void)
	: dataValueArray_(boost::make_shared<OpcUaDataValueArray>())
	, valueInfoEntry_()
	, statusCode_("")
	{
	}

	void
	HistoricalReadResponseData::statusCode(const std::string& statusCode)
	{
		statusCode_ = statusCode;
	}

	std::string&
	HistoricalReadResponseData::statusCode(void)
	{
		return statusCode_;
	}

	HistoricalReadResponseData::~HistoricalReadResponseData(void)
	{
	}

	OpcUaDataValueArray::SPtr&
	HistoricalReadResponseData::dataValueArray(void)
	{
		return dataValueArray_;
	}

	void
	HistoricalReadResponseData::valueInfoEntry(const ValueInfoEntry::SPtr& valueInfoEntry)
	{
		valueInfoEntry_ = valueInfoEntry;
	}

	ValueInfoEntry::SPtr&
	HistoricalReadResponseData::valueInfoEntry(void)
	{
		return valueInfoEntry_;
	}

	bool
	HistoricalReadResponseData::jsonEncode(boost::property_tree::ptree& pt)
	{
		if (statusCode_.length() != 0) {
			pt.put("StatusCode", statusCode_);
		}
		else {
			boost::property_tree::ptree array;
			if (!dataValueArray_->jsonEncode(array, "")) return false;
			pt.add_child("DataValues", array);
		}
		return true;
	}

	bool
	HistoricalReadResponseData::jsonDecode(boost::property_tree::ptree& pt)
	{
		return false;
	}


}


