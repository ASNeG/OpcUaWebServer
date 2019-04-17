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
#include "OpcUaStackCore/BuildInTypes/OpcUaDateTime.h"
#include "OpcUaWebServer/OpcUaClient/HistoricalReadRequestData.h"

using namespace OpcUaStackCore;

namespace OpcUaWebServer
{

	HistoricalReadRequestData::HistoricalReadRequestData(void)
	: variable_("")
	{
	}

	HistoricalReadRequestData::~HistoricalReadRequestData(void)
	{
	}

	void
	HistoricalReadRequestData::variable(const std::string& variable)
	{
		variable_ = variable;
	}

	std::string&
	HistoricalReadRequestData::variable(void)
	{
		return variable_;
	}

	void
	HistoricalReadRequestData::startTime(boost::posix_time::ptime startTime)
	{
		startTime_ = startTime;
	}

	boost::posix_time::ptime
	HistoricalReadRequestData::startTime(void)
	{
		return startTime_;
	}

	void
	HistoricalReadRequestData::endTime(boost::posix_time::ptime endTime)
	{
		endTime_ = endTime;
	}

	boost::posix_time::ptime
	HistoricalReadRequestData::endTime(void)
	{
		return endTime_;
	}

	bool
	HistoricalReadRequestData::decode(boost::property_tree::ptree& pt)
	{
		// get variable
		boost::optional<std::string> variable = pt.get_optional<std::string>("Variable");
		if (!variable) {
			Log(Error, "Variable do not exist in historical read request");
			return false;
		}
		variable_ = *variable;

		// get start time
		boost::optional<std::string> startTime = pt.get_optional<std::string>("StartTime");
		if (!startTime) {
			Log(Error, "StartTime do not exist in historical read request");
			return false;
		}
		OpcUaDateTime opcUaStartTime;
		if (!opcUaStartTime.fromISOString(*startTime)) {
			Log(Error, "StartTime format error in historical read request");
			return false;
		}
		startTime_ = opcUaStartTime.dateTime();

		// get end time
		boost::optional<std::string> endTime = pt.get_optional<std::string>("EndTime");
		if (!startTime) {
			Log(Error, "EndTime do not exist in historical read request");
			return false;
		}
		OpcUaDateTime opcUaEndTime;
		if (!opcUaEndTime.fromISOString(*endTime)) {
			Log(Error, "EndTime format error in historical read request");
			return false;
		}
		endTime_ = opcUaEndTime.dateTime();

		return true;
	}

	bool
	HistoricalReadRequestData::encode(boost::property_tree::ptree& pt)
	{
		return false;
	}

}


