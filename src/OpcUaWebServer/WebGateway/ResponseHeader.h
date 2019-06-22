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

#ifndef __OpcUaWebServer_ResponseHeader_h__
#define __OpcUaWebServer_ResponseHeader_h__

#include "OpcUaStackCore/BuildInTypes/OpcUaStatusCode.h"
#include "OpcUaWebServer/WebGateway/RequestHeader.h"

using namespace OpcUaStackCore;

namespace OpcUaWebServer
{

	class ResponseHeader
	: public RequestHeader
	{
	  public:
		ResponseHeader(void);
		ResponseHeader(const RequestHeader& RequestHeader);
		ResponseHeader(
		    const OpcUaString& messageType,
			const OpcUaString& clientHandle,
			const OpcUaString& sessionId
		);
		virtual ~ResponseHeader(void);

		OpcUaStatusCode& statusCode(void);

      protected:
        bool jsonEncodeImpl(boost::property_tree::ptree& pt) const override;
        bool jsonDecodeImpl(const boost::property_tree::ptree& pt) override;

	  private:
		OpcUaStatusCode statusCode_;
	};

}

#endif
