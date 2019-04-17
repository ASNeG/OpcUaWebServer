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

#ifndef __OpcUaWebServer_ReadRequestData_h__
#define __OpcUaWebServer_ReadRequestData_h__

#include <boost/property_tree/ptree.hpp>
#include <string>

namespace OpcUaWebServer
{

	class ReadRequestData
	{
	  public:
		ReadRequestData(void);
		~ReadRequestData(void);

		void variable(const std::string& variable);
		std::string& variable(void);

		bool decode(boost::property_tree::ptree& pt);
		bool encode(boost::property_tree::ptree& pt);

	  private:
		std::string variable_;
	};

}

#endif
