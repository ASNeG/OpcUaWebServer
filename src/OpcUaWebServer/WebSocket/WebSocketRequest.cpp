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

#include <iostream>

#include "OpcUaStackCore/Base/Log.h"
#include "OpcUaWebServer/WebSocket/WebSocketRequest.h"

using namespace OpcUaStackCore;

namespace OpcUaWebServer
{

	WebSocketRequest::WebSocketRequest(void)
	{
	}

	WebSocketRequest::~WebSocketRequest(void)
	{
	}

	std::string
	WebSocketRequest::method(void)
	{
		return method_;
	}

	std::string
	WebSocketRequest::path(void)
	{
		return path_;
	}

	std::string
	WebSocketRequest::httpVersion(void)
	{
		return httpVersion_;
	}

	void
	WebSocketRequest::content(const std::string& content)
	{
		content_ = content;
	}

	std::string&
	WebSocketRequest::content(void)
	{
		return content_;
	}

	void
	WebSocketRequest::clear(void)
	{
		method_ = "";
		path_ = "";
		httpVersion_ = "";
		headerMap_.clear();
	}

	bool
	WebSocketRequest::decodeRequestHeader(std::istream& is)
	{
		std::string line;
		clear();

		//
		// read header line from http request
		//
        getline(is, line);
        size_t methodEnd = line.find(' ');
        size_t pathEnd = line.find(' ', methodEnd+1);
        if (methodEnd == std::string::npos || pathEnd == std::string::npos) {
        	Log(Error, "invalid header line in http request");
        	return false;
        }

        method_ = line.substr(0, methodEnd);
        path_ = line.substr(methodEnd+1, pathEnd-methodEnd-1);
        httpVersion_ = line.substr(pathEnd+6, line.size()-pathEnd-7);

        Log log(Debug, "receive http request");
        log.parameter("Method", method_);
        log.parameter("Path", path_);
        log.parameter("HttpVersion", httpVersion_);

        //
        // read parameter from http request
        //
        getline(is, line);
        size_t paramEnd = line.find(':');
        while(paramEnd != std::string::npos) {
            size_t valueStart = paramEnd + 1;
            if(line[valueStart] == ' ') valueStart++;

            std::string key = line.substr(0, paramEnd);
            std::string para = line.substr(valueStart, line.size()-valueStart-1);
            headerMap_.insert(std::make_pair(key, para));
            log.parameter(key, para);

            getline(is, line);
            paramEnd = line.find(':');
        }

        return true;
	}

	bool
	WebSocketRequest::getHeaderPara(const std::string& key, std::string& para)
	{
		std::map<std::string, std::string>::iterator it;
		it = headerMap_.find(key);
		if (it == headerMap_.end()) return false;
		para = it->second;
		return true;
	}

	bool
	WebSocketRequest::setHeaderPara(const std::string& key, const std::string& para)
	{
		std::map<std::string, std::string>::iterator it;
		it = headerMap_.find(key);
		if (it != headerMap_.end()) return false;
		headerMap_.insert(std::make_pair(key, para));
		return true;
	}

	std::map<std::string, std::string>&
	WebSocketRequest::headerMap()
	{
		return headerMap_;
	}

}
