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

#include "OpcUaStackCore/Base/Log.h"
#include "OpcUaWebServer/OpcUaClient/MonitorUpdateData.h"

using namespace OpcUaStackCore;

namespace OpcUaWebServer
{

	MonitorUpdateData::MonitorUpdateData(void)
	: dataValue_()
	, valueInfoEntry_()
	{
	}

	MonitorUpdateData::~MonitorUpdateData(void)
	{
	}

	void
	MonitorUpdateData::dataValue(OpcUaDataValue::SPtr dataValue)
	{
		dataValue_ = dataValue;
	}

	OpcUaDataValue::SPtr
	MonitorUpdateData::dataValue(void)
	{
		return dataValue_;
	}

	void
	MonitorUpdateData::valueInfoEntry(const ValueInfoEntry::SPtr& valueInfoEntry)
	{
		valueInfoEntry_ = valueInfoEntry;
	}

	ValueInfoEntry::SPtr&
	MonitorUpdateData::valueInfoEntry(void)
	{
		return valueInfoEntry_;
	}

	bool
	MonitorUpdateData::jsonDecode(boost::property_tree::ptree& pt)
	{
		return false;
	}

	bool
	MonitorUpdateData::jsonEncode(boost::property_tree::ptree& pt)
	{
		if (!dataValue_->jsonEncode(pt)) return false;
		return true;
	}

}


