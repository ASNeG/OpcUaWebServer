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
#include "OpcUaStackCore/BuildInTypes/OpcUaString.h"
#include "OpcUaWebServer/OpcUaClient/ValueInfoResponseData.h"

using namespace OpcUaStackCore;

namespace OpcUaWebServer
{

	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	//
	// ValueInfoData
	//
	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	ValueInfoData::ValueInfoData(void)
	: statusCode_("")
	, variableName_("")
	, variableType_("")
	, isArray_(false)
	, metaData_()
	{
	}

	ValueInfoData::~ValueInfoData(void)
	{
	}

	void
	ValueInfoData::statusCode(const std::string& statusCode)
	{
		statusCode_ = statusCode;
	}

	std::string&
	ValueInfoData::statusCode(void)
	{
		return statusCode_;
	}

	void
	ValueInfoData::variableName(const std::string& variableName)
	{
		variableName_ = variableName;
	}

	std::string&
	ValueInfoData::variableName(void)
	{
		return variableName_;
	}

	void
	ValueInfoData::variableType(const std::string& variableType)
	{
		variableType_ = variableType;
	}

	std::string&
	ValueInfoData::variableType(void)
	{
		return variableType_;
	}

	void
	ValueInfoData::isArray(bool isArray)
	{
		isArray_ = isArray;
	}
	bool
	ValueInfoData::isArray(void)
	{
		return isArray_;
	}

	void
	ValueInfoData::metaData(const boost::property_tree::ptree& metaData)
	{
		metaData_ = metaData;
	}

	boost::property_tree::ptree&
	ValueInfoData::metaData(void)
	{
		return metaData_;
	}

	bool
	ValueInfoData::encode(boost::property_tree::ptree& pt)
	{
		pt.put("Variable", variableName_);

		if (statusCode_.length() != 0) {
			pt.put("StatusCode", statusCode_);
		}
		else {
			pt.put("Type", variableType_);
			boost::property_tree::ptree isArray;
			Json::encode(isArray, (OpcUaBoolean)isArray_);
			pt.add_child("IsArray", isArray);
			if (metaData_.size() != 0) {
				pt.add_child("MetaData", metaData_);
			}
		}
		return true;
	}

	bool
	ValueInfoData::decode(boost::property_tree::ptree& pt)
	{
		return false;
	}

	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	//
	// ValueInfoResponseData
	//
	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	ValueInfoResponseData::ValueInfoResponseData(void)
	: valueInfoDataVec_()
	, statusCode_("")
	{
	}

	ValueInfoResponseData::~ValueInfoResponseData(void)
	{
	}

	void
	ValueInfoResponseData::statusCode(const std::string& statusCode)
	{
		statusCode_ = statusCode;
	}

	std::string&
	ValueInfoResponseData::statusCode(void)
	{
		return statusCode_;
	}

	void
	ValueInfoResponseData::valueInfoDataVec(ValueInfoData::Vec& valueInfoDataVec)
	{
		valueInfoDataVec_ = valueInfoDataVec;
	}

	ValueInfoData::Vec&
	ValueInfoResponseData::valueInfoDataVec(void)
	{
		return valueInfoDataVec_;
	}

	bool
	ValueInfoResponseData::encode(boost::property_tree::ptree& pt)
	{
		// set status code
		if (statusCode_.length() != 0) {
			pt.put("StatusCode", statusCode_);
		}

		// set value array
		boost::property_tree::ptree array;
		ValueInfoData::Vec::iterator it;
		for (it = valueInfoDataVec_.begin(); it != valueInfoDataVec_.end(); it++) {
			boost::property_tree::ptree arrayElement;
			it->encode(arrayElement);

			array.push_back(std::make_pair("", arrayElement));
		}
		pt.add_child("Variables", array);
		return true;
	}

	bool
	ValueInfoResponseData::decode(boost::property_tree::ptree& pt)
	{
		return false;
	}

}


