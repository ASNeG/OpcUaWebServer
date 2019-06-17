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
#include "OpcUaWebServer/WebGateway/SessionStatusNotify.h"

using namespace OpcUaStackCore;

namespace OpcUaWebServer
{

	SessionStatusNotify::SessionStatusNotify(void)
	: sessionStatus_("")
	{
	}

	SessionStatusNotify::~SessionStatusNotify(void)
	{
	}

	OpcUaString&
	SessionStatusNotify::sessionStatus(void)
	{
		return sessionStatus_;
	}

	bool
	SessionStatusNotify::jsonEncodeImpl(boost::property_tree::ptree& pt) const
	{
		return jsonObjectEncode(pt, sessionStatus_, "SessionStatus");
	}

	bool
	SessionStatusNotify::jsonDecodeImpl(const boost::property_tree::ptree& pt)
	{
		return jsonObjectDecode(pt, sessionStatus_, "SessionStatus");
	}

}

