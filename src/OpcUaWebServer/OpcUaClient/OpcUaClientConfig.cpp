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

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include "OpcUaStackCore/Base/Log.h"
#include "OpcUaWebServer/OpcUaClient/OpcUaClientConfig.h"
#include "OpcUaWebServer/OpcUaClient/PtreeConverter.h"

using namespace OpcUaStackCore;

namespace OpcUaWebServer
{

	// -----------------------------------------------------------------------
	// -----------------------------------------------------------------------
	//
	// OpcUaClientEndpoint
	//
	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	OpcUaClientEndpoint::OpcUaClientEndpoint(void)
	: serverUri_("")
	{
	}

	OpcUaClientEndpoint::~OpcUaClientEndpoint(void)
	{
	}


	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	//
	// NamesapceUri
	//
	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	NamespaceUri::NamespaceUri(void)
	: uriVec_()
	{
	}

	NamespaceUri::~NamespaceUri(void)
	{
	}


	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	//
	// NodeEntry
	//
	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	NodeEntry::NodeEntry(void)
	: valueName_("")
	, nodeId_()
	, type_(OpcUaBuildInType_Unknown)
	, array_(false)
	{
	}

	NodeEntry::~NodeEntry(void)
	{
	}


	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	//
	// NodeList
	//
	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	NodeList::NodeList(void)
	: nodeEntryVec_()
	{
	}

	NodeList::~NodeList(void)
	{
	}

	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	//
	// OpcUaClientConfiguration
	//
	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	OpcUaClientConfig::OpcUaClientConfig(void)
	: Object()
	, clientConfigFile_("")
	, name_("Unknown")
	, opcUaClientEndpoint_()
	, namespaceUri_()
	, nodeList_()
	{
	}

	OpcUaClientConfig::~OpcUaClientConfig(void)
	{
	}

	bool
	OpcUaClientConfig::decode(const std::string& clientConfigFile)
	{
		clientConfigFile_ = clientConfigFile;
		boost::property_tree::ptree pt;
		std::string errorMessage = "";
		bool error = false;

		// parse client configuration file
		try {
			boost::property_tree::read_xml(clientConfigFile, pt);
		}
		catch (const boost::property_tree::xml_parser_error& e)
		{
			Log(Error, "parse client configuration error")
				.parameter("What", e.what())
				.parameter("ClientConfigFile", clientConfigFile);
			return false;

			errorMessage = std::string(e.what());
			error = true;
		}

		if (error) {
			Log(Error, "parse client configuration error")
				.parameter("ClientConfigFile", clientConfigFile);
			return false;
		}

		// get OpcUaClient from client configuration
		boost::optional<boost::property_tree::ptree&> client = pt.get_child_optional("OpcUaClient");
		if (!client) {
			Log(Error, "node not found in client configuration file")
				.parameter("NodeName", "OpcUaClient")
				.parameter("ConfigurationFileName", clientConfigFile_);
		}

		// get name
		boost::optional<std::string> name = client->get_optional<std::string>("<xmlattr>.Name");
		if (!name) {
			Log(Error, "attribute not exist in client configuration")
				.parameter("NodeName", "OpcUaClient")
				.parameter("AttributeName", "Name")
				.parameter("ConfigurationFileName", clientConfigFile_);
			return false;
		}
		name_ = *name;

		bool existEndpoint = false;
		bool existNamespaceUri = false;
		bool existNodeList = false;

		boost::property_tree::ptree::iterator it;
		for (it = client->begin(); it != client->end(); it++) {
			// decode Endpoint
			if (it->first == "Endpoint") {
				if (existEndpoint) {
					Log(Error, "duplicate Endpoint found in client configuration")
						.parameter("Name", name_)
						.parameter("NodePath", "OpcUaClient")
						.parameter("ConfigurationFileName", clientConfigFile_);
					return false;
				}

				if (!decodeEndpoint(it->second)) return true;
				existEndpoint = true;
			}

			// decode NamespaceUri
			else if (it->first == "NamespaceUri") {
				if (existNamespaceUri) {
					Log(Error, "duplicate NamespaceUri found in client configuration")
						.parameter("Name", name_)
						.parameter("NodePath", "OpcUaClient")
						.parameter("ConfigurationFileName", clientConfigFile_);
					return false;
				}

				if (!decodeNamespaceUri(it->second)) return false;
				existNamespaceUri = true;
			}

			// decode NodeList
			else if (it->first == "NodeList") {
				if (existNodeList) {
					Log(Error, "duplicate NodeList found in client configuration")
						.parameter("Name", name_)
						.parameter("NodePath", "OpcUaClient")
						.parameter("ConfigurationFileName", clientConfigFile_);
					return false;
				}

				if (!decodeNodeList(it->second)) return false;
				existNodeList = true;
			}

			else if (it->first == "<xmlattr>") {
				// nothing to do
			}

			// unknown node
			else {
				Log(Warning, "found invalid node in client configuration")
					.parameter("NodePath", "OpcUaClient")
					.parameter("NodeName", it->first)
					.parameter("ConfigurationFileName", clientConfigFile_);
			}
		}

		if (!existEndpoint) {
			Log(Error, "Endpoint not exist in client configuration")
				.parameter("Name", name_)
				.parameter("NodePath", "OpcUaClient")
				.parameter("ConfigurationFileName", clientConfigFile_);
			return false;
		}

		if (!existNamespaceUri) {
			Log(Error, "NamespaceUri not exist in client configuration")
				.parameter("Name", name_)
				.parameter("NodePath", "OpcUaClient")
				.parameter("ConfigurationFileName", clientConfigFile_);
			return false;
		}

		if (!existNodeList) {
			Log(Error, "NodeList not exist in client configuration")
				.parameter("Name", name_)
				.parameter("NodePath", "OpcUaClient")
				.parameter("ConfigurationFileName", clientConfigFile_);
			return false;
		}

		return true;
	}

	bool
	OpcUaClientConfig::decodeEndpoint(boost::property_tree::ptree& pt)
	{
		bool existServerUri = false;

		boost::property_tree::ptree::iterator it;
		for (it = pt.begin(); it != pt.end(); it++) {
			if (it->first == "ServerUri") {
				if (existServerUri) {
					Log(Error, "duplicate Uri found in client configuration")
						.parameter("Name", name_)
						.parameter("NodePath", "OpcUaClient.Endpoint")
						.parameter("ConfigurationFileName", clientConfigFile_);
					return false;
				}

				opcUaClientEndpoint_.serverUri_ = it->second.data();
				existServerUri = true;
			}

			else if (it->first == "SecurityPolicyUri") {
				if (!SecurityPolicy::exist(it->second.data())) {
					Log(Error, "security policy uri invalid in client configuration")
						.parameter("Name", name_)
						.parameter("NodePath", "OpcUaClient.SecurityPolicyUri")
						.parameter("SecurityPolicyUri", it->second.data())
						.parameter("ConfigurationFileName", clientConfigFile_);
					return false;
				}
				opcUaClientEndpoint_.securityPolicy_ = SecurityPolicy::str2Enum(it->second.data());
			}

			else if (it->first == "MessageSecurityMode") {
				Log(Error, "message security mode invalid in client configuration")
					.parameter("Name", name_)
					.parameter("NodePath", "OpcUaClient.MessageSecurityMode")
					.parameter("MessageSecurityMode", it->second.data())
					.parameter("ConfigurationFileName", clientConfigFile_);
				opcUaClientEndpoint_.securityMode_ = MessageSecurityMode::str2Enum(it->second.data());
			}

			else {
				Log(Warning, "found invalid node in client configuration")
					.parameter("NodePath", "OpcUaClient.Endpoint")
					.parameter("NodeName", it->first)
					.parameter("ConfigurationFileName", clientConfigFile_);
			}
		}

		if (!existServerUri) {
			Log(Error, "ServerUri not exist in client configuration")
				.parameter("Name", name_)
				.parameter("NodePath", "OpcUaClient.Endpoint")
				.parameter("ConfigurationFileName", clientConfigFile_);
			return false;
		}

		return true;
	}

	bool
	OpcUaClientConfig::decodeNamespaceUri(boost::property_tree::ptree& pt)
	{
		bool existUri = false;

		boost::property_tree::ptree::iterator it;
		for (it = pt.begin(); it != pt.end(); it++) {
			if (it->first == "Uri") {
				namespaceUri_.uriVec_.push_back(it->second.data());
				existUri = true;
			}

			else {
				Log(Warning, "found invalid node in client configuration")
					.parameter("NodePath", "OpcUaClient.NamespaceUri")
					.parameter("NodeName", it->first)
					.parameter("ConfigurationFileName", clientConfigFile_);
			}
		}

		if (!existUri) {
			Log(Error, "Uri not exist in client configuration")
				.parameter("Name", name_)
				.parameter("NodePath", "OpcUaClient.NamespaceUri")
				.parameter("ConfigurationFileName", clientConfigFile_);
			return false;
		}

		return true;
	}

	bool
	OpcUaClientConfig::decodeNodeList(boost::property_tree::ptree& pt)
	{
		boost::property_tree::ptree::iterator it;
		for (it = pt.begin(); it != pt.end(); it++) {
			if (it->first == "Node") {
				NodeEntry nodeEntry;

				// get ValueName
				boost::optional<std::string> valueName = it->second.get_optional<std::string>("<xmlattr>.ValueName");
				if (!valueName) {
					Log(Error, "attribute not exist in client configuration")
						.parameter("NodePath", "OpcUaClient.NodeList.Node")
						.parameter("AttributeName", "ValueName")
						.parameter("ConfigurationFileName", clientConfigFile_);
					return false;
				}
				nodeEntry.valueName_ = *valueName;

				// get NodeId
				boost::optional<std::string> nodeId = it->second.get_optional<std::string>("<xmlattr>.NodeId");
				if (!nodeId) {
					Log(Error, "attribute not exist in client configuration")
						.parameter("NodePath", "OpcUaClient.NodeList.Node")
						.parameter("ValueName", nodeEntry.valueName_)
						.parameter("AttributeName", "NodeId")
						.parameter("ConfigurationFileName", clientConfigFile_);
					return false;
				}
				if (!nodeEntry.nodeId_.fromString(*nodeId)) {
					Log(Error, "invalid attribute value in client configuration")
						.parameter("NodePath", "OpcUaClient.NodeList.Node")
						.parameter("ValueName", nodeEntry.valueName_)
						.parameter("AttributeName", "NodeId")
						.parameter("NodeId", *nodeId)
						.parameter("ConfigurationFileName", clientConfigFile_);
					return false;
				}

				// get NodeType
				boost::optional<std::string> nodeType = it->second.get_optional<std::string>("<xmlattr>.NodeType");
				if (!nodeId) {
					Log(Error, "attribute not exist in client configuration")
						.parameter("NodePath", "OpcUaClient.NodeList.Node")
						.parameter("ValueName", nodeEntry.valueName_)
						.parameter("AttributeName", "NodeType")
						.parameter("ConfigurationFileName", clientConfigFile_);
					return false;
				}
				nodeEntry.type_ = OpcUaBuildInTypeMap::string2BuildInType(*nodeType);
				if (nodeEntry.type_ == OpcUaBuildInType_Unknown) {
					Log(Error, "invalid attribute value in client configuration")
						.parameter("NodePath", "OpcUaClient.NodeList.Node")
						.parameter("ValueName", nodeEntry.valueName_)
						.parameter("AttributeName", "NodeType")
						.parameter("NodeType", *nodeType)
						.parameter("ConfigurationFileName", clientConfigFile_);
					return false;
				}

				// get Array (optional)
				boost::optional<std::string> array = it->second.get_optional<std::string>("<xmlattr>.Array");
				if (array && *array == "1") nodeEntry.array_ = true;

				// get meta data (optional)
				boost::optional<boost::property_tree::ptree&> metaData;
				metaData = it->second.get_child_optional("MetaData");

				if (metaData) {
					boost::property_tree::ptree json;
					PtreeConverter ptreeConverter;
					ptreeConverter.xml2Json(*metaData, json);
					nodeEntry.metaData_ = json;
				}

				nodeList_.nodeEntryVec_.push_back(nodeEntry);
			}

			else {
				Log(Warning, "found invalid node in client configuration")
					.parameter("NodePath", "OpcUaClient.NodeList")
					.parameter("NodeName", it->first)
					.parameter("ConfigurationFileName", clientConfigFile_);
			}
		}

		return true;
	}

}
