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
#include <boost/make_shared.hpp>
#include "OpcUaStackCore/Base/Log.h"
#include "OpcUaWebServer/OpcUaClient/ReadResponseData.h"

using namespace OpcUaStackCore;

namespace OpcUaWebServer
{

	ReadResponseData::ReadResponseData(void)
	: dataValue_()
	, valueInfoEntry_()
	, statusCode_("")
	{
	}

	void
	ReadResponseData::statusCode(const std::string& statusCode)
	{
		statusCode_ = statusCode;
	}

	std::string&
	ReadResponseData::statusCode(void)
	{
		return statusCode_;
	}

	ReadResponseData::~ReadResponseData(void)
	{
	}

	void
	ReadResponseData::dataValue(OpcUaDataValue& dataValue)
	{
		OpcUaDataValue::SPtr dataValueTmp = boost::make_shared<OpcUaDataValue>();
	    dataValueTmp->copyFrom(dataValue);
	    this->dataValue(dataValueTmp);
	}

	void
	ReadResponseData::dataValue(OpcUaDataValue::SPtr dataValue)
	{
		dataValue_ = dataValue;
	}

	OpcUaDataValue::SPtr
	ReadResponseData::dataValue(void)
	{
		return dataValue_;
	}

	void
	ReadResponseData::valueInfoEntry(const ValueInfoEntry::SPtr& valueInfoEntry)
	{
		valueInfoEntry_ = valueInfoEntry;
	}

	ValueInfoEntry::SPtr&
	ReadResponseData::valueInfoEntry(void)
	{
		return valueInfoEntry_;
	}

	bool
	ReadResponseData::jsonDecode(boost::property_tree::ptree& pt)
	{
		return false;
	}

	bool
	ReadResponseData::jsonEncode(boost::property_tree::ptree& pt)
	{
		if (statusCode_.length() != 0) {
			pt.put("StatusCode", statusCode_);
		}
		else {
			if (!dataValue_->jsonEncode(pt)) return false;
		}
		return true;
	}

}


