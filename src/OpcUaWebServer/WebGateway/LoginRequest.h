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

#ifndef __OpcUaWebServer_LoginRequest_h__
#define __OpcUaWebServer_LoginRequest_h__

#include <map>
#include <boost/property_tree/ptree.hpp>
#include "OpcUaStackCore/StandardDataTypes/MessageSecurityMode.h"
#include "OpcUaStackCore/BuildInTypes/SecurityPolicy.h"

using namespace OpcUaStackCore;

namespace OpcUaWebServer
{

	enum class UserAuthType
	{
		Anonymous,
		UserName,
		X509,
		Issued
	};

	class UserAuthentication
	{
	  public:
		UserAuthentication(void);
		~UserAuthentication(void);

		void userAuthType(UserAuthType userAuthType);
		UserAuthType userAuthType(void);
		void insertAttribute(const std::string& name, const std::string& value);
		std::string getAttribute(const std::string& name);
		bool existAttribute(const std::string& name);
		std::map<std::string, std::string>& attributes(void);

	  private:
		UserAuthType userAuthType_;
		std::map<std::string, std::string> attributes_;
	};


	class LoginRequest
	{
	  public:
		LoginRequest(void);
		virtual ~LoginRequest(void);

		std::string& discoveryUrl(void);
		MessageSecurityMode::Enum securityMode(void);
		SecurityPolicy::Enum securityPolicy(void);
		UserAuthentication& userAuthentication(void);
		void log(const std::string& message);

		bool jsonEncode(boost::property_tree::ptree& pt);
		bool jsonDecode(boost::property_tree::ptree& pt);

	  private:
		std::string discoveryUrl_;
		MessageSecurityMode::Enum securityMode_;
		SecurityPolicy::Enum securityPolicy_;
		UserAuthentication userAuthentication_;
	};

}

#endif
