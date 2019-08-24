/*
   Copyright 2015-2017 Kai Huebl (kai@huebl-sgh.de)

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
#include "OpcUaWebServer/OpcUaClient/ValueInfo.h"

using namespace OpcUaStackCore;

namespace OpcUaWebServer
{

	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	//
	// ValueInfoEntry
	//
	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	ValueInfoEntry::ValueInfoEntry(void)
	: valueName_("")
	, nodeId_()
	, type_(OpcUaBuildInType_Unknown)
	, array_(false)
	, metaData_()
	{
	}

	ValueInfoEntry::~ValueInfoEntry(void)
	{
	}

	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	//
	// ValueInfo
	//
	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	ValueInfo::ValueInfo(void)
	: valueInfoEntryMap_()
	{
	}

	ValueInfo::~ValueInfo(void)
	{
	}

	bool
	ValueInfo::addClientConfig(OpcUaClientConfig::SPtr& opcUaClientConfig, const Object::SPtr& object)
	{
		NodeEntry::Vec::iterator it1;

		for (
			it1 = opcUaClientConfig->nodeList_.nodeEntryVec_.begin();
			it1 != opcUaClientConfig->nodeList_.nodeEntryVec_.end();
			it1++
		)
		{
			ValueInfoEntry::Map::iterator it2;
			it2 = valueInfoEntryMap_.find(it1->valueName_);
			if (it2 != valueInfoEntryMap_.end()) {
				Log(Error, "duplicate value name in client configuration found")
					.parameter("ValueName", it1->valueName_);
				return false;
			}

			ValueInfoEntry::SPtr valueInfoEntry = boost::make_shared<ValueInfoEntry>();
			valueInfoEntry->valueName_= it1->valueName_;
			valueInfoEntry->nodeId_ = it1->nodeId_;
			valueInfoEntry->type_ = it1->type_;
			valueInfoEntry->array_ = it1->array_;
			valueInfoEntry->metaData_ = it1->metaData_;
			valueInfoEntry->object_ = object;

			valueInfoEntryMap_.insert(std::make_pair(valueInfoEntry->valueName_, valueInfoEntry));
		}

		return true;
	}

	ValueInfoEntry::Map&
	ValueInfo::valueInfoEntryMap(void)
	{
		return valueInfoEntryMap_;
	}

	void
	ValueInfo::valueNameVec(std::vector<std::string>& valueNameVec)
	{
		ValueInfoEntry::Map::iterator it;
		for (it = valueInfoEntryMap_.begin(); it != valueInfoEntryMap_.end(); it++) {
			valueNameVec.push_back(it->first);
		}
	}

	ValueInfoEntry::SPtr
	ValueInfo::getValueInfo(const std::string& valueName)
	{
		ValueInfoEntry::Map::iterator it;
		ValueInfoEntry::SPtr valueInfoEntry;

		it = valueInfoEntryMap_.find(valueName);
		if (it != valueInfoEntryMap_.end()) {
			valueInfoEntry = it->second;
		}

		return valueInfoEntry;
	}

}
