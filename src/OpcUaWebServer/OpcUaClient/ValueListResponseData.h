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

#ifndef __OpcUaWebServer_ValueListResponseData_h__
#define __OpcUaWebServer_ValueListResponseData_h__

#include <boost/property_tree/ptree.hpp>
#include <string>

namespace OpcUaWebServer
{

	class ValueListResponseData
	{
	  public:
		ValueListResponseData(void);
		~ValueListResponseData(void);

		void valueVec(std::vector<std::string>& valueVec);
		std::vector<std::string>& valueVec(void);

		bool jsonDecode(boost::property_tree::ptree& pt);
		bool jsonEncode(boost::property_tree::ptree& pt);

	  private:
		std::vector<std::string> valueVec_;
	};

}

#endif
