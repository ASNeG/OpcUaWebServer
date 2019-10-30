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

#ifndef __OpcUaWebServer_RequestHeader_h__
#define __OpcUaWebServer_RequestHeader_h__

#include <stdint.h>
#include <boost/property_tree/ptree.hpp>
#include "OpcUaStackCore/BuildInTypes/OpcUaString.h"

using namespace OpcUaStackCore;

namespace OpcUaWebServer
{

	class RequestHeader
	: public JsonFormatter
	{
	  public:
		RequestHeader(void);
		RequestHeader(const RequestHeader& RequestHeader);
		RequestHeader(
		    const OpcUaString& messageType,
			const OpcUaString& clientHandle,
			const OpcUaString& sessionId,
			uint32_t requestTimeout
		);
		virtual ~RequestHeader(void);

		OpcUaString& messageType(void);
		OpcUaString& clientHandle(void);
		OpcUaString& sessionId(void);
		uint32_t requestTimeout(void) const;

      protected:
        bool jsonEncodeImpl(boost::property_tree::ptree& pt) const override;
        bool jsonDecodeImpl(const boost::property_tree::ptree& pt) override;

	  private:
        OpcUaString messageType_;
        OpcUaString clientHandle_;
        OpcUaString sessionId_;
        uint32_t requestTimeout_;
	};

}

#endif
