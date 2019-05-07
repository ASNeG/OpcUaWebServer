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

#include "OpcUaStackCore/Base/Log.h"
#include "OpcUaStackCore/BuildInTypes/JsonNumber.h"
#include "OpcUaWebServer/WebGateway/SubscriptionStatusNotify.h"

using namespace OpcUaStackCore;

namespace OpcUaWebServer
{

	SubscriptionStatusNotify::SubscriptionStatusNotify(void)
	: subscriptionId_(0)
	, subscriptionStatus_("")
	{
	}

	SubscriptionStatusNotify::~SubscriptionStatusNotify(void)
	{
	}

	uint32_t&
	SubscriptionStatusNotify::subscriptionId(void)
	{
		return subscriptionId_;
	}

	std::string&
	SubscriptionStatusNotify::subscriptionStatus(void)
	{
		return subscriptionStatus_;
	}

	bool
	SubscriptionStatusNotify::jsonEncode(boost::property_tree::ptree& pt)
	{
		JsonNumber::jsonEncode(pt, subscriptionId_, "SubscriptionId");
		pt.put("SubscriptionStatus", subscriptionStatus_);
		return true;
	}

	bool
	SubscriptionStatusNotify::jsonDecode(boost::property_tree::ptree& pt)
	{
		// get subscription id
		if (!JsonNumber::jsonDecode(pt, subscriptionId_, "SubscriptionId")) {
			Log(Error, "message body do not contain subscription id");
			return false;
		}

		// get session status from json message
		boost::optional<std::string> subscriptionStatus = pt.get_optional<std::string>("SubscriptionStatus");
		if (!subscriptionStatus) {
			Log(Error, "message body do not contain subscription status");
			return false;
		}
		subscriptionStatus_ = *subscriptionStatus;

		return true;
	}

}

