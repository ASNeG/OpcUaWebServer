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

#ifndef __OpcUaWebServer_HttpRequest_h__
#define __OpcUaWebServer_HttpRequest_h__

#include <istream>
#include <map>

namespace OpcUaWebServer
{

	class HttpRequest
	{
	  public:
		HttpRequest(void);
		~HttpRequest(void);

		std::string method(void);
		std::string path(void);
		std::string httpVersion(void);
		void content(const std::string& content);
		std::string& content(void);

		void clear(void);
		bool decodeRequestHeader(std::istream& is);
		bool getHeaderPara(const std::string& key, std::string& para);
		std::map<std::string, std::string>& headerMap();

	  private:
		std::string method_;
		std::string path_;
		std::string httpVersion_;
		std::map<std::string, std::string> headerMap_;
		std::string content_;
	};

}

#endif
