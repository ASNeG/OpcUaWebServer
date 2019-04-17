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
#include "OpcUaWebServer/WebSocket/WebSocketResponse.h"

using namespace OpcUaStackCore;

namespace OpcUaWebServer
{

	WebSocketResponse::WebSocketResponse(void)
	: statusCode_(200)
	, statusString_("OK")
	, content_("")
	{
	}

	WebSocketResponse::~WebSocketResponse(void)
	{
	}

	void
	WebSocketResponse::statusCode(uint32_t statusCode)
	{
		statusCode_ = statusCode;
	}

	uint32_t
	WebSocketResponse::statusCode(void)
	{
		return statusCode_;
	}

	void
	WebSocketResponse::statusString(const std::string& statusString)
	{
		statusString_ = statusString;
	}

	std::string
	WebSocketResponse::statusString(void)
	{
		return statusString_;
	}

	void
	WebSocketResponse::content(const std::string& content)
	{
		content_ = content;
	}

	std::string&
	WebSocketResponse::content(void)
	{
		return content_;
	}

	void
	WebSocketResponse::clear(void)
	{
		statusCode_ = 200;
		statusString_ = "OK";
		headerMap_.clear();
	}

	bool
	WebSocketResponse::encodeRequestHeader(std::ostream& os)
	{
		Log log(Debug, "send http response");
		log.parameter("StatusCode", statusCode_);
		log.parameter("StatusString", statusString_);

		// output header
		os << "HTTP/1.1 " << statusCode_ << " " << statusString_ << "\r\n";

		// output parameter
		std::map<std::string, std::string>::iterator it;
		for (it = headerMap_.begin(); it != headerMap_.end(); it++) {
			log.parameter(it->first, it->second);
			os << it->first << ": " << it->second << "\r\n";
		}

		if (content_.length() == 0) {
			os << "\r\n";
			return true;
		}

		log.parameter("ContentLength", content_.length());
		os << "ContentLength: " << content_.length() << "\r\n";
		os << "\r\n";
		os << content_;
		return true;
	}

	bool
	WebSocketResponse::setHeaderPara(const std::string& key, const std::string& para)
	{
		std::map<std::string, std::string>::iterator it;
		it = headerMap_.find(key);
		if (it != headerMap_.end()) return false;
		headerMap_.insert(std::make_pair(key, para));
		return true;
	}

	bool
	WebSocketResponse::getHeaderPara(const std::string& key, std::string& para)
	{
		std::map<std::string, std::string>::iterator it;
		it = headerMap_.find(key);
		if (it == headerMap_.end()) return false;
		para = it->second;
		return true;
	}

}
