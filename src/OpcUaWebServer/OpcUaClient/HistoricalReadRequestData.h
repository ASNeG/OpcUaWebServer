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

#ifndef __OpcUaWebServer_HistoricalReadRequestData_h__
#define __OpcUaWebServer_HistoricalReadRequestData_h__

#include <boost/property_tree/ptree.hpp>
#include "boost/date_time/posix_time/posix_time.hpp"
#include <string>

namespace OpcUaWebServer
{

	class HistoricalReadRequestData
	{
	  public:
		HistoricalReadRequestData(void);
		~HistoricalReadRequestData(void);

		void variable(const std::string& variable);
		std::string& variable(void);
		void startTime(boost::posix_time::ptime startTime);
		boost::posix_time::ptime startTime(void);
		void endTime(boost::posix_time::ptime endTime);
		boost::posix_time::ptime endTime(void);

		bool decode(boost::property_tree::ptree& pt);
		bool encode(boost::property_tree::ptree& pt);

	  private:
		std::string variable_;
		boost::posix_time::ptime startTime_;
		boost::posix_time::ptime endTime_;
	};

}

#endif
