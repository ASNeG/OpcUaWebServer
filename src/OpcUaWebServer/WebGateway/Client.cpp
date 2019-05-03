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

#include "OpcUaWebServer/WebGateway/Client.h"
#include "OpcUaWebServer/WebGateway/LoginRequest.h"
#include "OpcUaWebServer/WebGateway/LogoutRequest.h"

namespace OpcUaWebServer
{

	uint32_t Client::gId_  = 0;

	Client::Client(void)
	: id_(++gId_)
	, sessionStatusCallback_()
	, logoutResponseCallback_()
	, serviceSetManager_()
	, ioThread_()
	, cryptoManager_()
	, sessionService_()
	{
	}

	Client::~Client(void)
	{
	}

	std::string
	Client::id(void)
	{
		std::stringstream ss;
		ss << id_;
		return ss.str();
	}


	void
	Client::ioThread(IOThread::SPtr& ioThread)
	{
		ioThread_ = ioThread;
	}

	void
	Client::cryptoManager(CryptoManager::SPtr& cryptoManager)
	{
		cryptoManager_ = cryptoManager;
	}

	OpcUaStatusCode
	Client::login(
		boost::property_tree::ptree& requestBody,
		boost::property_tree::ptree& responseBody,
		const SessionStatusCallback& sessionStatusCallback
	)
	{
		sessionStatusCallback_ = sessionStatusCallback;

		// parse login request
		LoginRequest loginRequest;
		if (!loginRequest.jsonDecode(requestBody)) {
			Log(Error, "decode login request error")
				.parameter("Id", id_);
			return BadInvalidArgument;
		}

		// set secure channel configuration
		SessionServiceConfig sessionServiceConfig;
		sessionServiceConfig.secureChannelClient_->discoveryUrl(loginRequest.discoveryUrl());
		sessionServiceConfig.secureChannelClient_->cryptoManager(cryptoManager_);
		sessionServiceConfig.session_->sessionName("WebGateway");
		sessionServiceConfig.sessionServiceChangeHandler_ =
			[this] (SessionBase& session, SessionServiceStateId sessionState) {
				if (sessionState == SessionServiceStateId::Established) {
					sessionStatusCallback_("Connect");
				}
				else if (sessionState == SessionServiceStateId::Disconnected) {
					sessionStatusCallback_("Disconnect");

					if (logoutResponseCallback_) {
						boost::property_tree::ptree responseBody;
						logoutResponseCallback_(Success, responseBody);
					}
				}
			};

		// create session
		sessionService_ = serviceSetManager_.sessionService(sessionServiceConfig);
		if (sessionService_.get() == nullptr) {
			Log(Error, "create session error")
				.parameter("Id", id_);
			return BadInvalidArgument;
		}

		// open a connection to the opc ua server
		sessionService_->asyncConnect();

		// create response body
		responseBody.put("SessionId", id_);
		return Success;
	}

	void
	Client::logout(
		boost::property_tree::ptree& requestBody,
		const LogoutResponseCallback& logoutResponseCallback
	)
	{
		boost::property_tree::ptree responseBody;

		// check parameter
		if (logoutResponseCallback_) {
			Log(Error, "logout function already called")
				.parameter("Id", id_);
			logoutResponseCallback(BadInvalidArgument, responseBody);
			return;
		}
		logoutResponseCallback_ = logoutResponseCallback;

		// parse logout request
		LogoutRequest logoutRequest;
		if (!logoutRequest.jsonDecode(requestBody)) {
			Log(Error, "decode logout request error")
				.parameter("Id", id_);
			logoutResponseCallback(BadInvalidArgument, responseBody);
			return;
		}

		// close the connection to the opc ua server
		sessionService_->asyncDisconnect();
	}

}
