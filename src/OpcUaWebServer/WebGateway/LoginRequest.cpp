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

#include <OpcUaWebServer/WebGateway/LoginRequest.h>
#include "OpcUaStackCore/Base/Log.h"

using namespace OpcUaStackCore;

namespace OpcUaWebServer
{

	LoginRequest::LoginRequest(void)
	: discoveryUrl_("")
	{
	}

	LoginRequest::~LoginRequest(void)
	{
	}


	bool
	LoginRequest::jsonEncode(boost::property_tree::ptree& pt)
	{
		boost::property_tree::ptree bodyElement;
		bodyElement.put("DiscoveryUrl", discoveryUrl_);
		pt.push_back(std::make_pair("Body", bodyElement));

		return true;
	}

	bool
	LoginRequest::jsonDecode(boost::property_tree::ptree& pt)
	{
		// get discoery url from json message
		boost::optional<std::string> discoveryUrl = pt.get_optional<std::string>("Body.DiscoveryUrl");
		if (!discoveryUrl) {
			Log(Error, "message body do not contain discovery url");
			return false;
		}
		discoveryUrl_ = *discoveryUrl;

		return true;
	}

}

