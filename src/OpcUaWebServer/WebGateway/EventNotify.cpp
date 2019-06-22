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
#include "OpcUaWebServer/WebGateway/EventNotify.h"

using namespace OpcUaStackCore;

namespace OpcUaWebServer
{

	EventNotify::EventNotify(void)
	: clientHandle_(0)
	, event_()
	{
	}

	EventNotify::~EventNotify(void)
	{
	}

	uint32_t&
	EventNotify::clientHandle(void)
	{
		return clientHandle_;
	}

	OpcUaVariantArray&
	EventNotify::event(void)
	{
		return event_;
	}

    bool
	EventNotify::jsonEncodeImpl(boost::property_tree::ptree& pt) const
    {
    	bool rc = true;
    	rc = rc & jsonNumberEncode(pt, clientHandle_, "ClientHandleData");
    	rc = rc & jsonArrayEncode(pt, event_, "Event");
    	return rc;
    }

    bool
	EventNotify::jsonDecodeImpl(const boost::property_tree::ptree& pt)
    {
    	bool rc = true;
    	rc = rc & jsonNumberDecode(pt, clientHandle_, "ClientHandleData");
    	rc = rc & jsonArrayDecode(pt, event_, "Event");
    	return rc;
    }

}

