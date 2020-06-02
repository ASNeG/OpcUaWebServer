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

#ifndef __OpcUaWebServer_OpcUaClientConfig_h__
#define __OpcUaWebServer_OpcUaClientConfig_h__

#include <boost/shared_ptr.hpp>
#include <boost/property_tree/ptree.hpp>
#include <vector>
#include <map>
#include "OpcUaStackCore/BuildInTypes/OpcUaNodeId.h"
#include "OpcUaStackCore/BuildInTypes/OpcUaType.h"
#include "OpcUaStackCore/StandardDataTypes/MessageSecurityMode.h"
#include "OpcUaStackCore/StandardDataTypes/UserTokenType.h"
#include "OpcUaStackCore/BuildInTypes/SecurityPolicy.h"

namespace OpcUaWebServer
{

    class OpcUaClientAuthentication
	{
	  public:
    	OpcUaClientAuthentication(void);
    	~OpcUaClientAuthentication(void);

    	bool decode(boost::property_tree::ptree& pt);

    	OpcUaStackCore::UserTokenType::Enum userTokenType_ = OpcUaStackCore::UserTokenType::EnumAnonymous;
    	std::string policyId_ = "";
    	std::string userName_ = "";
    	std::string password_ = "";
    	OpcUaStackCore::SecurityPolicy::Enum securityPolicy_ = OpcUaStackCore::SecurityPolicy::EnumNone;
	};

	class OpcUaClientEndpoint
	{
	  public:
		OpcUaClientEndpoint(void);
		~OpcUaClientEndpoint(void);

		bool decode(boost::property_tree::ptree& pt);

		std::string discoveryUrl_ = "";
		std::string endpointUrl_ = "";
		std::string applicationUri_ = "";
		OpcUaStackCore::MessageSecurityMode::Enum securityMode_ = OpcUaStackCore::MessageSecurityMode::EnumNone;
		OpcUaStackCore::SecurityPolicy::Enum securityPolicy_ = OpcUaStackCore::SecurityPolicy::EnumNone;

		OpcUaClientAuthentication authentication_;
	};


	class NamespaceUri
	{
	  public:
		NamespaceUri(void);
		~NamespaceUri(void);

		bool decode(boost::property_tree::ptree& pt);

		std::vector<std::string> uriVec_;
	};


	class NodeEntry
	{
	  public:
		typedef std::vector<NodeEntry> Vec;

		NodeEntry(void);
		~NodeEntry(void);

		bool decode(boost::property_tree::ptree& pt);

		std::string valueName_;
		OpcUaStackCore::OpcUaNodeId nodeId_;
		OpcUaStackCore::OpcUaBuildInType type_;
		bool array_;
		boost::property_tree::ptree metaData_;
	};


	class NodeList
	{
	  public:
		NodeList(void);
		~NodeList(void);

		bool decode(boost::property_tree::ptree& pt);

		NodeEntry::Vec nodeEntryVec_;
	};


	class OpcUaClientConfig
	: public OpcUaStackCore::Object
	{
	  public:
		typedef boost::shared_ptr<OpcUaClientConfig> SPtr;
		typedef std::map<std::string, OpcUaClientConfig::SPtr> Map;
		typedef std::vector<OpcUaClientConfig::SPtr> Vec;

		OpcUaClientConfig(void);
		~OpcUaClientConfig(void);

		bool decode(const std::string& clientConfigFile);

	  public:
		std::string name_;
		OpcUaClientEndpoint opcUaClientEndpoint_;
		NamespaceUri namespaceUri_;
		NodeList nodeList_;

	  private:
		std::string clientConfigFile_;
	};

}

#endif
