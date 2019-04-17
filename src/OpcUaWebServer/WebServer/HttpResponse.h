/*
   Copyright 2015 Kai Huebl (kai@huebl-sgh.de)

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

#ifndef __OpcUaWebServer_HttpResponse_h__
#define __OpcUaWebServer_HttpResponse_h__

#include <istream>
#include <map>
#include <stdint.h>

namespace OpcUaWebServer
{

	class HttpResponse
	{
	  public:
		HttpResponse(void);
		~HttpResponse(void);

		void statusCode(uint32_t statusCode);
		uint32_t statusCode(void);
		void statusString(const std::string& statusString);
		std::string statusString(void);
		void content(const std::string& content);
		std::string& content(void);

		void clear(void);
		bool setHeaderPara(const std::string& key, const std::string& para);
		bool getHeaderPara(const std::string& key, std::string& para);
		bool encodeRequestHeader(std::ostream& os);

	  private:
		uint32_t statusCode_;
		std::string statusString_;
		std::string content_;
		std::map<std::string, std::string> headerMap_;
	};

}

#endif
