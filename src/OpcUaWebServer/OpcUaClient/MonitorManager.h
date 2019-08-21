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

#ifndef __OpcUaWebServer_MonitorManager_h__
#define __OpcUaWebServer_MonitorManager_h__

#include <boost/shared_ptr.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <map>
#include <set>
#include <stdint.h>
#include "OpcUaWebServer/OpcUaClient/ValueInfo.h"

namespace OpcUaWebServer
{

	class MonitorItem
	{
	  public:
		typedef boost::shared_ptr<MonitorItem> SPtr;
		typedef std::map<std::string, MonitorItem::SPtr> Map;
		typedef std::map<uint32_t, MonitorItem::SPtr> IdMap;
		typedef std::vector<MonitorItem::SPtr> Vec;
		typedef std::set<uint32_t> ChannelIdSet;

		typedef enum
		{
			MIS_Close,
			MIS_Creating,
			MIS_Open
		} MonitorItemState;

		static uint32_t gClientHandle_;

		MonitorItem(void);
		~MonitorItem(void);

		ChannelIdSet& channelIdSet(void);
		void valueInfoEntry(const ValueInfoEntry::SPtr& valueInfoEntry);
		ValueInfoEntry::SPtr& valueInfoEntry(void);
		void monitorItemState(MonitorItemState monitorItemState);
		MonitorItemState monitorItemState(void);
		uint32_t clientHandle(void);
		void reconnectTime(boost::posix_time::ptime& reconnectTime);
		boost::posix_time::ptime& reconnectTime(void);
		void monitoredItemId(uint32_t monitoredItemId);
		uint32_t monitoredItemId(void);

	  private:
		MonitorItemState monitorItemState_;
		uint32_t clientHandle_;
		boost::posix_time::ptime reconnectTime_;
		uint32_t monitoredItemId_;

		ChannelIdSet channelIdSet_;
		ValueInfoEntry::SPtr valueInfoEntry_;
	};

	class MonitorManager
	{
	  public:
		MonitorManager(void);
		~MonitorManager(void);

		MonitorItem::Map& monitorItemMap(void);

		bool addMonitorItem(uint32_t channelId, const std::string& variable, const ValueInfoEntry::SPtr& valueInfoEntry);
		bool delMonitorItem(uint32_t channelId, const std::string& variable);
		bool delMonitorItem(uint32_t channelId);
		MonitorItem::SPtr getMonitorItem(uint32_t clientHandle);

	  private:
		MonitorItem::Map monitorItemMap_;
		MonitorItem::IdMap monitorItemIdMap_;
	};

}

#endif
