/*
   Copyright 2019-2020 Kai Huebl (kai@huebl-sgh.de)

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
#include "OpcUaStackCore/BuildInTypes/JsonFormatter.h"
#include "OpcUaStackClient/ServiceSet/SessionMode.h"

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
	: public OpcUaStackCore::JsonFormatter
	{
	  public:
		LoginRequest(void);
		virtual ~LoginRequest(void);

		OpcUaStackCore::OpcUaString& discoveryUrl(void);
		OpcUaStackClient::SessionMode sessionMode(void);
		OpcUaStackCore::MessageSecurityMode::Enum securityMode(void);
		OpcUaStackCore::SecurityPolicy::Enum securityPolicy(void);
		UserAuthentication& userAuthentication(void);

		void log(const std::string& message);

	  protected:
		bool jsonEncodeImpl(boost::property_tree::ptree& pt) const override;
		bool jsonDecodeImpl(const boost::property_tree::ptree& pt) override;

	  private:
		OpcUaStackClient::SessionMode sessionMode_ = OpcUaStackClient::SessionMode::SecureChannelAndSession;
		OpcUaStackCore::OpcUaString discoveryUrl_;
		OpcUaStackCore::MessageSecurityMode::Enum securityMode_;
		OpcUaStackCore::SecurityPolicy::Enum securityPolicy_;
		UserAuthentication userAuthentication_;
	};

}

#endif
