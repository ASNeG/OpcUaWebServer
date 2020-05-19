/*
   Copyright 2015-2020 Kai Huebl (kai@huebl-sgh.de)

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

#ifndef __OpcUaWebServer_ValueInfoResponseData_h__
#define __OpcUaWebServer_ValueInfoResponseData_h__

#include <boost/property_tree/ptree.hpp>
#include <string>
#include "OpcUaStackCore/BuildInTypes/OpcUaDataValue.h"

namespace OpcUaWebServer
{

	class ValueInfoData
	{
	  public:
		typedef std::vector<ValueInfoData> Vec;

		ValueInfoData(void);
		~ValueInfoData(void);

		void statusCode(const std::string& statusCode);
		std::string& statusCode(void);
		void variableName(const std::string& variableName);
		std::string& variableName(void);
		void variableType(const std::string& variableType);
		std::string& variableType(void);
		void isArray(bool isArray);
		bool isArray(void);
		void metaData(const boost::property_tree::ptree& metaData);
		boost::property_tree::ptree& metaData(void);

		bool encode(boost::property_tree::ptree& pt);
		bool decode(boost::property_tree::ptree& pt);

	  private:
		std::string statusCode_;
		std::string variableName_;
		std::string variableType_;
		bool isArray_;
		boost::property_tree::ptree metaData_;
	};

	class ValueInfoResponseData
	{
	  public:
		ValueInfoResponseData(void);
		~ValueInfoResponseData(void);

		void statusCode(const std::string& statusCode);
		std::string& statusCode(void);
		void valueInfoDataVec(ValueInfoData::Vec& valueInfoDataVec);
		ValueInfoData::Vec& valueInfoDataVec(void);

		bool encode(boost::property_tree::ptree& pt);
		bool decode(boost::property_tree::ptree& pt);

	  private:
		std::string statusCode_;
		ValueInfoData::Vec valueInfoDataVec_;
	};

}

#endif
