/*
   Copyright 2019 Kai Huebl (kai@huebl-sgh.de)

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
#include "OpcUaWebServer/OpcUaClient/MonitorManager.h"

namespace OpcUaWebServer
{

	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	//
	// MonitorItem
	//
	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------

	uint32_t MonitorItem::gClientHandle_ = 0;

	MonitorItem::MonitorItem(void)
	: channelIdSet_()
	, valueInfoEntry_()
	, monitorItemState_(MIS_Close)
	, clientHandle_(++gClientHandle_)
	, reconnectTime_(boost::posix_time::microsec_clock::universal_time())
	{
	}

	MonitorItem::~MonitorItem(void)
	{
	}

	MonitorItem::ChannelIdSet&
	MonitorItem::channelIdSet(void)
	{
		return channelIdSet_;
	}

	void
	MonitorItem::valueInfoEntry(const ValueInfoEntry::SPtr& valueInfoEntry)
	{
		valueInfoEntry_ = valueInfoEntry;
	}

	ValueInfoEntry::SPtr&
	MonitorItem::valueInfoEntry(void)
	{
		return valueInfoEntry_;
	}

	void
	MonitorItem::monitorItemState(MonitorItemState monitorItemState)
	{
		monitorItemState_ = monitorItemState;
	}

	MonitorItem::MonitorItemState
	MonitorItem::monitorItemState(void)
	{
		return monitorItemState_;
	}

	uint32_t
	MonitorItem::clientHandle(void)
	{
		return clientHandle_;
	}

	void
	MonitorItem::reconnectTime(boost::posix_time::ptime& reconnectTime)
	{
		reconnectTime_ = reconnectTime;
	}

	boost::posix_time::ptime&
	MonitorItem::reconnectTime(void)
	{
		return reconnectTime_;
	}

	void
	MonitorItem::monitoredItemId(uint32_t monitoredItemId)
	{
		monitoredItemId_ = monitoredItemId;
	}

	uint32_t
	MonitorItem::monitoredItemId(void)
	{
		return monitoredItemId_;
	}

	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	//
	// MonitorManager
	//
	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	MonitorManager::MonitorManager(void)
	: monitorItemMap_()
	, monitorItemIdMap_()
	{
	}

	MonitorManager::~MonitorManager(void)
	{
	}

	MonitorItem::Map&
	MonitorManager::monitorItemMap(void)
	{
		return monitorItemMap_;
	}

	bool
	MonitorManager::addMonitorItem(uint32_t channelId, const std::string& variable, const ValueInfoEntry::SPtr& valueInfoEntry)
	{
		// get or create monitor item
		MonitorItem::SPtr monitorItem;
		MonitorItem::Map::iterator it1;
		it1 = monitorItemMap_.find(variable);
		if (it1 == monitorItemMap_.end()) {
			monitorItem = boost::make_shared<MonitorItem>();
			monitorItem->valueInfoEntry(valueInfoEntry);
			monitorItemMap_.insert(std::make_pair(variable, monitorItem));
			monitorItemIdMap_.insert(std::make_pair(monitorItem->clientHandle(), monitorItem));
		}
		else {
			monitorItem = it1->second;
		}

		// add channel id if necessary
		MonitorItem::ChannelIdSet::iterator it2;
		it2 = monitorItem->channelIdSet().find(channelId);
		if (it2 != monitorItem->channelIdSet().end()) {
			return false;
		}

		// add channel id
		monitorItem->channelIdSet().insert(channelId);
		return true;
	}

	bool
	MonitorManager::delMonitorItem(uint32_t channelId, const std::string& variable)
	{
		// get monitor item
		MonitorItem::SPtr monitorItem;
		MonitorItem::Map::iterator it1;
		it1 = monitorItemMap_.find(variable);
		if (it1 == monitorItemMap_.end()) {
			return false;
		}
		monitorItem = it1->second;

		// get channel id
		MonitorItem::ChannelIdSet::iterator it2;
		it2 = monitorItem->channelIdSet().find(channelId);
		if (it2 == monitorItem->channelIdSet().end()) {
			return false;
		}

		// remove channel id
		monitorItem->channelIdSet().erase(it2);
		if (monitorItem->channelIdSet().size() == 0) {
			monitorItemMap_.erase(it1);
			monitorItemIdMap_.erase(monitorItem->clientHandle());
		}

		return true;
	}

	bool
	MonitorManager::delMonitorItem(uint32_t channelId)
	{
		bool success = true;
		MonitorItem::Map::iterator it;
		for (it = monitorItemMap_.begin(); it != monitorItemMap_.end(); it++) {
			if (!delMonitorItem(channelId, it->first)) success = false;
		}
		return success;
	}

	MonitorItem::SPtr
	MonitorManager::getMonitorItem(uint32_t clientHandle)
	{
		MonitorItem::IdMap::iterator it;
		it = monitorItemIdMap_.find(clientHandle);
		if (it != monitorItemIdMap_.end()) return it->second;
		MonitorItem::SPtr monitorItem;
		return monitorItem;
	}

}
