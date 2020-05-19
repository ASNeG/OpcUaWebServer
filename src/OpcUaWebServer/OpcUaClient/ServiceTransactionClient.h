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

#ifndef __OpcUaWebServer_ServiceTransactionClient_h__
#define __OpcUaWebServer_ServiceTransactionClient_h__

#include <boost/shared_ptr.hpp>
#include <set>
#include "OpcUaStackClient/ServiceSet/AttributeService.h"
#include "OpcUaWebServer/OpcUaClient/ValueInfo.h"

namespace OpcUaWebServer
{

	class ServiceTransactionClientRead
	: public OpcUaStackCore::ServiceTransactionRead
	{
	  public:
		typedef boost::shared_ptr<ServiceTransactionClientRead> SPtr;

		ServiceTransactionClientRead(void);
		~ServiceTransactionClientRead(void);

		uint32_t channelId_;
		std::string clientHandle_;
		std::string variable_;
		ValueInfoEntry::SPtr valueInfoEntry_;

		// FIXME: temp..
		bool polling_;
		std::set<uint32_t> channelIdSet_;
	};


	class ServiceTransactionClientWrite
	: public OpcUaStackCore::ServiceTransactionWrite
	{
	  public:
		typedef boost::shared_ptr<ServiceTransactionClientWrite> SPtr;

		ServiceTransactionClientWrite(void);
		~ServiceTransactionClientWrite(void);

		uint32_t channelId_;
		std::string clientHandle_;
		std::string variable_;
	};

}

#endif
