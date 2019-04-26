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

#include "OpcUaWebServer/WS/WSConfig.h"

namespace OpcUaWebServer
{

	WSConfig::WSConfig(void)
	: address_("0.0.0.0")
	, port_(8082)
	{
	}

	WSConfig::WSConfig(const WSConfig& wsConfig)
	: address_(wsConfig.address_)
	, port_(wsConfig.port_)
	{
	}

	WSConfig::~WSConfig(void)
	{
	}

	void
	WSConfig::address(const std::string& address)
	{
		address_ = address;
	}

	std::string&
	WSConfig::address(void)
	{
		return address_;
	}

	void
	WSConfig::port(uint32_t port)
	{
		port_ = port;
	}

	uint32_t
	WSConfig::port(void)
	{
		return port_;
	}

	WSConfig&
	WSConfig::operator= (WSConfig const& rhs)
	{
		address_ = rhs.address_;
		port_ = rhs.port_;
		return *this;
	}

}
