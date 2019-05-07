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
#include "OpcUaWebServer/WebGateway/DataChangeNotify.h"

using namespace OpcUaStackCore;

namespace OpcUaWebServer
{

	DataChangeNotify::DataChangeNotify(void)
	: clientHandle_(0)
	, dataValue_()
	{
	}

	DataChangeNotify::~DataChangeNotify(void)
	{
	}

	uint32_t&
	DataChangeNotify::clientHandle(void)
	{
		return clientHandle_;
	}

	OpcUaDataValue&
	DataChangeNotify::dataValue(void)
	{
		return dataValue_;
	}

	bool
	DataChangeNotify::jsonEncode(boost::property_tree::ptree& pt)
	{
		JsonNumber::jsonEncode(pt, clientHandle_, "ClientHandleData");
		dataValue_.jsonEncode(pt, "Value");

		return true;
	}

	bool
	DataChangeNotify::jsonDecode(boost::property_tree::ptree& pt)
	{
		// get client handle
		if (!JsonNumber::jsonDecode(pt, clientHandle_, "ClientHandleData")) {
			Log(Error, "message body do not contain client handle");
			return false;
		}

		// get data value
		if (!dataValue_.jsonDecode(pt, "Value")) {
			Log(Error, "message body do not contain value");
			return false;
		}

		return true;
	}

}

