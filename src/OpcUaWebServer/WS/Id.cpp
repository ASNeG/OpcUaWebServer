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

#include "OpcUaWebServer/WS/Id.h"

namespace OpcUaWebServer
{

	uint32_t Id::gId_ = 0;
	boost::mutex Id::mutex_;

	Id::Id(const std::string& identifier)
	{
		std::stringstream ss;

		boost::mutex::scoped_lock g(mutex_);
		gId_++;
		ss << identifier;
		if (!identifier.empty()) ss << "-";
		ss << gId_;

		id_ = ss.str();
	}

	Id::~Id(void)
	{
	}

	std::string&
	Id::id(void)
	{
		return id_;
	}

}
