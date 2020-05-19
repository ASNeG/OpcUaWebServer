/*
   Copyright 2015-20120 Kai Huebl (kai@huebl-sgh.de)

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

#ifndef __OpcUaWebServer_ValueInfo_h__
#define __OpcUaWebServer_ValueInfo_h__

#include <boost/property_tree/ptree.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <map>
#include "OpcUaStackCore/BuildInTypes/OpcUaNodeId.h"
#include "OpcUaStackCore/BuildInTypes/OpcUaType.h"
#include "OpcUaWebServer/OpcUaClient/OpcUaClientConfig.h"

namespace OpcUaWebServer
{

	class ValueInfoEntry
	: public OpcUaStackCore::Object
	{
	  public:
		typedef boost::shared_ptr<ValueInfoEntry> SPtr;
		typedef std::map<std::string, ValueInfoEntry::SPtr> Map;

		ValueInfoEntry(void);
		~ValueInfoEntry(void);

		// entry info
		uint32_t id_;
		std::string valueName_;
		OpcUaStackCore::OpcUaNodeId nodeId_;
		OpcUaStackCore::OpcUaBuildInType type_;
		bool array_;
		boost::property_tree::ptree metaData_;

		Object::SPtr object_;
	};


	class ValueInfo
	{
	  public:
		ValueInfo(void);
		~ValueInfo(void);

		bool addClientConfig(OpcUaClientConfig::SPtr& opcUaClientConfig, const OpcUaStackCore::Object::SPtr& object);
		ValueInfoEntry::Map& valueInfoEntryMap(void);

		void valueNameVec(std::vector<std::string>& valueNameVec);
		ValueInfoEntry::SPtr getValueInfo(const std::string& valueName);

	  private:
		typedef std::map<uint32_t, ValueInfoEntry::SPtr> ValueInfoEntryIdMap;

		ValueInfoEntry::Map valueInfoEntryMap_;
	};

}

#endif
