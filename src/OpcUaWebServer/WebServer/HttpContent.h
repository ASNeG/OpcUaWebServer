/*
   Copyright 2015-2019 Kai Huebl (kai@huebl-sgh.de)

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

#ifndef __OpcUaWebServer_HttpContent_h__
#define __OpcUaWebServer_HttpContent_h__

#include "OpcUaStackCore/Base/Object.h"
#include "OpcUaWebServer/WebServer/HttpServerIf.h"
#include "OpcUaWebServer/WebServer/HttpConfig.h"

namespace OpcUaWebServer
{

	class HttpContent
	: public OpcUaStackCore::Object
	, public HttpServerIf
	{
	  public:
		typedef boost::shared_ptr<HttpContent> SPtr;

		HttpContent(void);
		virtual ~HttpContent(void);

		void httpConfig(HttpConfig* httpConfig);
		bool startup(void);
		bool shutdown(void);

		//- WebServerIf -------------------------------------------------------
		virtual void request(HttpRequest& httpRequest, HttpResponse& httpResponse);
		//- WebServerIf -------------------------------------------------------

	  private:
		bool checkPath(const std::string& path);
		HttpConfig* httpConfig_;
	};

}

#endif
