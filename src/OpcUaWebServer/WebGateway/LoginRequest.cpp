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

#include <OpcUaWebServer/WebGateway/LoginRequest.h>
#include "OpcUaStackCore/Base/Log.h"

using namespace OpcUaStackCore;
using namespace OpcUaStackClient;

namespace OpcUaWebServer
{

	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	//
	// UserAuthentication
	//
	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	UserAuthentication::UserAuthentication(void)
	: userAuthType_(UserAuthType::Anonymous)
	, attributes_()
	{
	}

	UserAuthentication::~UserAuthentication(void)
	{
	}

	void
	UserAuthentication::userAuthType(UserAuthType userAuthType)
	{
		userAuthType_ = userAuthType;
	}

	UserAuthType
	UserAuthentication::userAuthType(void)
	{
		return userAuthType_;
	}

	void
	UserAuthentication::insertAttribute(const std::string& name, const std::string& value)
	{
		attributes_.insert(std::make_pair(name, value));
	}

	std::string
	UserAuthentication::getAttribute(const std::string& name)
	{
		auto it = attributes_.find(name);
		if (it == attributes_.end()) {
			return "";
		}
		return it->second;
	}

	bool
	UserAuthentication::existAttribute(const std::string& name)
	{
		auto it = attributes_.find(name);
		return it != attributes_.end();
	}

	std::map<std::string, std::string>&
	UserAuthentication::attributes(void)
	{
		return attributes_;
	}

	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	//
	// LoginRequest
	//
	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	LoginRequest::LoginRequest(void)
	: discoveryUrl_("")
	, userAuthentication_()
	{
	}

	LoginRequest::~LoginRequest(void)
	{
	}

	UserAuthentication&
	LoginRequest::userAuthentication(void)
	{
		return userAuthentication_;
	}

	OpcUaString&
	LoginRequest::discoveryUrl(void)
	{
		return discoveryUrl_;
	}

	SessionMode
	LoginRequest::sessionMode(void)
	{
		return sessionMode_;
	}

	MessageSecurityMode::Enum
	LoginRequest::securityMode(void)
	{
		return securityMode_;
	}

	SecurityPolicy::Enum
	LoginRequest::securityPolicy(void)
	{
		return securityPolicy_;
	}

	void
	LoginRequest::log(const std::string& message)
	{
		Log log(Debug, message);
		log.parameter("DiscoveryUrl", discoveryUrl_);
		log.parameter("SecurityMode", securityMode_);
		log.parameter("SecurityPolicy", securityPolicy_);

		std::stringstream ss;
		bool first = true;
		for (auto attribute : userAuthentication_.attributes()) {
			if (attribute.first == "Password") continue;
			if (!first) ss << ",";
			first = false;
			ss << attribute.first << "=" << attribute.second;
		}
		log.parameter("UserAuthentication", ss.str());
	}

	bool
	LoginRequest::jsonEncodeImpl(boost::property_tree::ptree& pt) const
	{
		return true;
	}

	bool
	LoginRequest::jsonDecodeImpl(const boost::property_tree::ptree& pt)
	{
		bool rc = true;
		rc = rc & jsonObjectDecode(pt, discoveryUrl_, "DiscoveryUrl");

		// get security policy uri
		auto securityPolicyUri = pt.get_optional<std::string>("SecurityPolicyUri");
		if (securityPolicyUri) {
			if (SecurityPolicy::exist(*securityPolicyUri)) {
				securityPolicy_ = SecurityPolicy::str2Enum(*securityPolicyUri);
			}
			else {
				Log(Error, "LoginRequest decode error, because unknown security policy uri")
					.parameter("Element", "SecurityPolicyUri")
					.parameter("Value", *securityPolicyUri);
				return false;
			}
		}
		else {
			securityPolicy_ = SecurityPolicy::EnumNone;
		}

		// get session mode
		auto sessionMode = pt.get_optional<std::string>("SessionMode");
		if (sessionMode)
		{
			if (*sessionMode == "SecureChannelAndSession") {
				sessionMode_ = SessionMode::SecureChannelAndSession;
			}
			else if (*sessionMode == "SecureChannel") {
				sessionMode_ = SessionMode::SecureChannel;
			}
			else {
				Log(Error, "LoginRequest decode error, because unknown session mode")
					.parameter("Element", "SessionMode")
					.parameter("Value", *sessionMode);
				return false;
			}
		}

		// get security mode
		auto securityMode = pt.get_optional<std::string>("SecurityMode");
		if (securityMode) {
			if (MessageSecurityMode::exist(*securityMode)) {
				securityMode_ = MessageSecurityMode::str2Enum(*securityMode);
			}
			else {
				Log(Error, "LoginRequest decode error")
					.parameter("Element", "SecurityMode")
					.parameter("Value", *securityMode);
				return false;
			}
		}
		else {
			securityMode_ = MessageSecurityMode::EnumNone;
		}

		// get user auth
		auto userAuth = pt.get_optional<std::string>("UserAuth");
		if (!userAuth) {
			Log(Debug, "element UserAuth not exist; use anonymous user authentication");
			userAuthentication_.userAuthType(UserAuthType::Anonymous);
			return true;
		}
		auto userAuthType = pt.get_optional<std::string>("UserAuth.Type");
		if (!userAuthType) {
			Log(Debug, "element UserAuth.Type not exist; use anonymous user authentication");
			userAuthentication_.userAuthType(UserAuthType::Anonymous);
			return true;
		}

		// get policy id
		auto policyId = pt.get_optional<std::string>("UserAuth.PolicyId");
		if (!policyId) {
			userAuthentication_.insertAttribute("PolicyId", "");
		}
		else {
			userAuthentication_.insertAttribute("PolicyId", *policyId);
		}

		if (*userAuthType == "Anonymous") {
			// nothing to do
		}
		else if (*userAuthType == "UserName") {
			userAuthentication_.userAuthType(UserAuthType::UserName);

			// get user name
			auto userName = pt.get_optional<std::string>("UserAuth.UserName");
			if (!userName) {
				Log(Error, "LoginRequest decode error")
					.parameter("Element", "UserName");
				return false;
			}
			userAuthentication_.insertAttribute("UserName", *userName);

			// get password
			auto password = pt.get_optional<std::string>("UserAuth.Password");
			if (!password) {
				Log(Error, "LoginRequest decode error")
					.parameter("Element", "Password");
				return false;
			}
			userAuthentication_.insertAttribute("Password", *password);

			// get security policy uri
			auto securityPolicyUri = pt.get_optional<std::string>("UserAuth.SecurityPolicyUri");
			if (securityPolicyUri) {
				if (!SecurityPolicy::exist(*securityPolicyUri)) {
					Log(Error, "LoginRequest decode error")
						.parameter("Element", "UserAuth.SecurityPolicyUri")
						.parameter("Value", *securityPolicyUri);
					return false;
				}
				userAuthentication_.insertAttribute("SecurityPolicyUri", *securityPolicyUri);
			}
			else {
				userAuthentication_.insertAttribute("SecurityPolicyUri", "http://opcfoundation.org/UA/SecurityPolicy#None");
			}
		}
		else if (*userAuthType == "Certificate") {
			userAuthentication_.userAuthType(UserAuthType::X509);

			// get certificate
			auto certificate = pt.get_optional<std::string>("UserAuth.Certificate");
			if (!certificate) {
				Log(Error, "LoginRequest decode error")
					.parameter("Element", "Certificate");
				return false;
			}
			userAuthentication_.insertAttribute("Certificate", *certificate);

			// get private key
			auto privateKey = pt.get_optional<std::string>("UserAuth.PrivateKey");
			if (!privateKey) {
				Log(Error, "LoginRequest decode error")
					.parameter("Element", "PrivateKey");
				return false;
			}
			userAuthentication_.insertAttribute("PrivateKey", *privateKey);

			// get security policy uri
			auto securityPolicyUri = pt.get_optional<std::string>("UserAuth.SecurityPolicyUri");
			if (securityPolicyUri) {
				if (!SecurityPolicy::exist(*securityPolicyUri)) {
					Log(Error, "LoginRequest decode error")
						.parameter("Element", "UserAuth.SecurityPolicyUri")
						.parameter("Value", *securityPolicyUri);
					return false;
				}
				userAuthentication_.insertAttribute("SecurityPolicyUri", *securityPolicyUri);
			}
			else {
				userAuthentication_.insertAttribute("SecurityPolicyUri", "http://opcfoundation.org/UA/SecurityPolicy#None");
			}
		}
		else if (*userAuthType == "Issued") {
			userAuthentication_.userAuthType(UserAuthType::Issued);

			// get token data
			auto tokenData = pt.get_optional<std::string>("UserAuth.TokenData");
			if (!tokenData) {
				Log(Error, "LoginRequest decode error")
					.parameter("Element", "TokenData");
				return false;
			}
			userAuthentication_.insertAttribute("TokenData", *tokenData);

			// get security policy uri
			auto securityPolicyUri = pt.get_optional<std::string>("UserAuth.SecurityPolicyUri");
			if (securityPolicyUri) {
				userAuthentication_.insertAttribute("SecurityPolicyUri", *securityPolicyUri);
			}
			else {
				userAuthentication_.insertAttribute("SecurityPolicyUri", "http://opcfoundation.org/UA/SecurityPolicy#None");
			}
		}
		else {
			Log(Error, "LoginRequest decode error")
				.parameter("Element", "UserAuth.Type")
				.parameter("Value", *userAuthType);
		}

		return true;
	}

}

