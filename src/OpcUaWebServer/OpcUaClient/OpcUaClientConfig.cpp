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
	// OpcUaClientAuthentication
	//
	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	OpcUaClientAuthentication::OpcUaClientAuthentication(void)
	{
	}

	OpcUaClientAuthentication::~OpcUaClientAuthentication(void)
	{
	}

	bool
	OpcUaClientAuthentication::decode(boost::property_tree::ptree& pt)
	{
		for (auto it = pt.begin(); it != pt.end(); it++) {

			// ignore comments
			if (it->first == "<xmlcomment>") {
				continue;
			}

			// decode TokenType
			else if (it->first == "TokenType") {
				if (!UserTokenType::exist(it->second.data())) {
					Log(Error, "found invalid parameter in client configuration")
						.parameter("ParameterName", it->first)
						.parameter("ParameterValue", it->second.data());
					return false;
				}
				userTokenType_ = UserTokenType::str2Enum(it->second.data());
			}

			// decode PolicyId
			else if (it->first == "PolicyId") {
				policyId_ = it->second.data();
			}

			// decode UserName
			else if (it->first == "UserName") {
				userName_ = it->second.data();
			}

			// decode password
			else if (it->first == "Password") {
				password_ = it->second.data();
			}

			// decode security policy
			else if (it->first == "SecurityPolicyUri") {
				if (!SecurityPolicy::exist(it->second.data())) {
					Log(Error, "found invalid parameter in client configuration")
						.parameter("ParameterName", it->first)
						.parameter("ParameterValue", it->second.data());
					return false;
				}
				securityPolicy_ = SecurityPolicy::str2Enum(it->second.data());
			}

			else {
				Log(Error, "found unknown parameter in client configuration")
					.parameter("ParameterName", it->first);
				return false;
			}
		}

		return true;
	}

	// -----------------------------------------------------------------------
	// -----------------------------------------------------------------------
	//
	// OpcUaClientEndpoint
	//
	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	OpcUaClientEndpoint::OpcUaClientEndpoint(void)
	{
	}

	OpcUaClientEndpoint::~OpcUaClientEndpoint(void)
	{
	}

	bool
	OpcUaClientEndpoint::decode(boost::property_tree::ptree& pt)
	{
		for (auto it = pt.begin(); it != pt.end(); it++) {
			// ignore comments
			if (it->first == "<xmlcomment>") {
				continue;
			}

			else if (it->first == "EndpointUrl") {
				endpointUrl_ = it->second.data();
			}

			else if (it->first == "DiscoveryUrl") {
				discoveryUrl_ = it->second.data();
			}

			else if (it->first == "ApplicationUri") {
				applicationUri_ = it->second.data();
			}

			else if (it->first == "SecurityPolicyUri") {
				if (!SecurityPolicy::exist(it->second.data())) {
					Log(Error, "found invalid parameter in client configuration")
						.parameter("ParameterName", it->first)
						.parameter("ParameterValue", it->second.data());
					return false;
				}
				securityPolicy_ = SecurityPolicy::str2Enum(it->second.data());
			}

			else if (it->first == "MessageSecurityMode") {
				if (!MessageSecurityMode::exist(it->second.data())) {
					Log(Error, "found invalid parameter in client configuration")
						.parameter("ParameterName", it->first)
						.parameter("ParameterValue", it->second.data());
					return false;
				}
				securityMode_ = MessageSecurityMode::str2Enum(it->second.data());
			}

			else if (it->first == "Authentication") {
				if (!authentication_.decode(it->second)) {
					Log(Error, "parameter error in client configuration")
						.parameter("ParameterName", it->first);
					return false;
				}
			}

			else {
				Log(Error, "found unknown parameter in client configuration")
					.parameter("ParameterName", it->first);
				return false;
			}
		}

		if (!endpointUrl_.empty()) {
			// if we use the endpoint url configuration mode the application uri must
			// be exist. However, this is only neccessary if a certificate is required.
			if (applicationUri_.empty()) {
				if (securityMode_ != OpcUaStackCore::MessageSecurityMode::EnumNone ||
					securityPolicy_ != OpcUaStackCore::SecurityPolicy::EnumNone
				) {
				    Log(Error, "ApplicationUri not exist in client configuration (endpoint configuration mode)");
				    return false;
				}
			}
		}
		else if (!discoveryUrl_.empty()) {
			// nothing to check
		}
		else {
			Log(Error, "EndpointUrl or DiscoveryUrl not exist in client configuration");
			return false;
		}

		return true;
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

	bool
	NamespaceUri::decode(boost::property_tree::ptree& pt)
	{
		bool existUri = false;

		for (auto it = pt.begin(); it != pt.end(); it++) {
			if (it->first == "Uri") {
				uriVec_.push_back(it->second.data());
				existUri = true;
			}

			else {
				Log(Error, "found unknown parameter in client configuration")
					.parameter("ParameterName", it->first);
			}
		}

		if (!existUri) {
			Log(Error, "Uri not exist in client configuration");
			return false;
		}

		return true;
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

	bool
	NodeEntry::decode(boost::property_tree::ptree& pt)
	{
		// get ValueName
		boost::optional<std::string> valueName = pt.get_optional<std::string>("<xmlattr>.ValueName");
		if (!valueName) {
			Log(Error, "attribute not exist in client configuration")
				.parameter("AttributeName", "ValueName");
			return false;
		}
		valueName_ = *valueName;

		// get NodeId
		boost::optional<std::string> nodeId = pt.get_optional<std::string>("<xmlattr>.NodeId");
		if (!nodeId) {
			Log(Error, "attribute not exist in client configuration")
				.parameter("ValueName", valueName_)
				.parameter("AttributeName", "NodeId");
			return false;
		}
		if (!nodeId_.fromString(*nodeId)) {
			Log(Error, "invalid attribute value in client configuration")
				.parameter("ValueName", valueName_)
				.parameter("ParameterValue", *nodeId)
				.parameter("AttributeName", "NodeId");
			return false;
		}

		// get NodeType
		boost::optional<std::string> nodeType = pt.get_optional<std::string>("<xmlattr>.NodeType");
		if (!nodeId) {
			Log(Error, "attribute not exist in client configuration")
				.parameter("ValueName", valueName_)
				.parameter("AttributeName", "NodeId");
			return false;
		}
		type_ = OpcUaBuildInTypeMap::string2BuildInType(*nodeType);
		if (type_ == OpcUaBuildInType_Unknown) {
			Log(Error, "invalid attribute value in client configuration")
				.parameter("ValueName", valueName_)
				.parameter("ParameterValue", *nodeType)
				.parameter("AttributeName", "NodeId");
			return false;
		}

		// get Array (optional)
		boost::optional<std::string> array = pt.get_optional<std::string>("<xmlattr>.Array");
		if (array && *array == "1") array_ = true;

		// get meta data (optional)
		boost::optional<boost::property_tree::ptree&> metaData;
		metaData = pt.get_child_optional("MetaData");

		if (metaData) {
			boost::property_tree::ptree json;
			PtreeConverter ptreeConverter;
			ptreeConverter.xml2Json(*metaData, json);
			metaData_ = json;
		}

		return true;
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

	bool
	NodeList::decode(boost::property_tree::ptree& pt)
	{
		for (auto it = pt.begin(); it != pt.end(); it++) {
			if (it->first == "Node") {
				NodeEntry nodeEntry;
				if (!nodeEntry.decode(it->second)) {
					Log(Error, "error in client configuration")
						.parameter("ParameterName", it->second.data());
					return false;
				}
				nodeEntryVec_.push_back(nodeEntry);
			}

			else {
				Log(Warning, "found unknown parameter in client configuration")
					.parameter("ParameterName", it->first);
			}
		}

		return true;
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

		for (auto it = client->begin(); it != client->end(); it++) {
			// decode Endpoint
			if (it->first == "Endpoint") {
				if (!opcUaClientEndpoint_.decode(it->second)) {
					Log(Error, "error in client configuration")
						.parameter("Name", name_)
						.parameter("ParameterName", "OpcUaClient.Endpoint")
						.parameter("ConfigurationFileName", clientConfigFile_);
					return false;
				}
				existEndpoint = true;
			}

			// decode NamespaceUri
			else if (it->first == "NamespaceUri") {
				if (!namespaceUri_.decode(it->second)) {
					Log(Error, "error in client configuration")
						.parameter("Name", name_)
						.parameter("ParameterName", "OpcUaClient.NamespaceUri")
						.parameter("ConfigurationFileName", clientConfigFile_);
					return false;
				}
				existNamespaceUri = true;
			}

			// decode NodeList
			else if (it->first == "NodeList") {
				if (!nodeList_.decode(it->second)) {
					Log(Error, "error in client configuration")
						.parameter("Name", name_)
						.parameter("ParameterName", "OpcUaClient.NodeList")
						.parameter("ConfigurationFileName", clientConfigFile_);
					return false;
				}
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

}
