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
	, policyId_("")
	{
	}

	LoginRequest::~LoginRequest(void)
	{
	}

	std::string&
	LoginRequest::policyId(void)
	{
		return policyId_;
	}

	std::string&
	LoginRequest::discoveryUrl(void)
	{
		return discoveryUrl_;
	}


	bool
	LoginRequest::jsonEncode(boost::property_tree::ptree& pt)
	{
		// set discovery url
		pt.put("DiscoveryUrl", discoveryUrl_);

		// set policy id
		if (policyId_ != "") {
			pt.put("PolicyId", policyId_);
			return false;
		}
		return true;
	}

	bool
	LoginRequest::jsonDecode(boost::property_tree::ptree& pt)
	{
		// get discovery url from json message
		auto discoveryUrl = pt.get_optional<std::string>("DiscoveryUrl");
		if (!discoveryUrl) {
			Log(Error, "LoginRequest decode error")
				.parameter("Element", "DiscoveryUrl");
			return false;
		}
		discoveryUrl_ = *discoveryUrl;

		// get policy id
		auto policyId = pt.get_optional<std::string>("PolicyId");
		if (policyId) {
			policyId_ = *policyId;
		}

		return true;
	}

}

