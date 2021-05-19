/*
   Copyright 2019-2021 Kai Huebl (kai@huebl-sgh.de)

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

#include <boost/make_shared.hpp>
#include "OpcUaStackCore/Base/ConfigJson.h"
#include "OpcUaStackCore/Base/Utility.h"
#include "OpcUaStackCore/Utility/UniqueId.h"
#include "OpcUaWebServer/WebGateway/Client.h"
#include "OpcUaWebServer/WebGateway/LoginRequest.h"
#include "OpcUaWebServer/WebGateway/LogoutRequest.h"

using namespace OpcUaStackCore;

namespace OpcUaWebServer
{

	uint32_t Client::gId_  = 0;

	Client::Client(void)
	: id_(++gId_)
	, sessionStatusCallback_()
	, logoutResponseCallback_()
	, subscriptionStatusCallback_()
	, dataChangeCallback_()
	, eventCallback_()
	, serviceSetManager_()
	, cryptoManager_()
	, sessionService_()
	, attributeService_()
	, methodService_()
	, subscriptionService_()
	, monitoredItemService_()
	, sessionName_()
	{
		std::stringstream sessionName;
		sessionName << "WebGateway-" << id_;
		sessionName_ = sessionName.str();
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
	Client::cryptoManager(CryptoManager::SPtr& cryptoManager)
	{
		cryptoManager_ = cryptoManager;
	}

    void
	Client::ioThread(OpcUaStackCore::IOThread::SPtr& ioThread)
    {
    	ioThread_ = ioThread;
		strand_ = ioThread_->createStrand();
    }

    void
	Client::messageBus(OpcUaStackCore::MessageBus::SPtr& messageBus)
    {
    	messageBus_ = messageBus;
    }

	OpcUaStatusCode
	Client::login(
		boost::property_tree::ptree& requestBody,
		boost::property_tree::ptree& responseBody,
		const SessionStatusCallback& sessionStatusCallback
	)
	{
		Log(Debug, "receive login request")
			.parameter("Id", id_);

		sessionStatusCallback_ = sessionStatusCallback;

		// session service state callback
		auto sessionStateUpdate =
		    [this] (SessionBase& session, SessionServiceStateId sessionState) {
			    if (sessionState != SessionServiceStateId::Established && sessionState != SessionServiceStateId::Disconnected) {
			    	return;
			    }

			    if (sessionState == SessionServiceStateId::Established) {
				    sessionStatusCallback_("Connect");
				}
				else {
				    if (logoutResponseCallback_) {
						boost::property_tree::ptree responseBody;
						logoutResponseCallback_(Success, responseBody);
						return;
					}

					sessionStatusCallback_("Disconnect");
				}
			};

		// parse login request
		LoginRequest loginRequest;
		if (!loginRequest.jsonDecode(requestBody)) {
			Log(Error, "decode login request error")
				.parameter("Id", id_);
			return BadInvalidArgument;
		}
		loginRequest.log("login request parameter");

		// register thread in service set manager. All services are using the
		// same thread pool
		serviceSetManager_.registerIOThread(ioThread_->name(), ioThread_);
		serviceSetManager_.messageBus(messageBus_);

		// create session service
		SessionServiceConfig sessionServiceConfig;
		sessionServiceConfig.secureChannelClient_->discoveryUrl(loginRequest.discoveryUrl());
		sessionServiceConfig.secureChannelClient_->cryptoManager(cryptoManager_);
		sessionServiceConfig.secureChannelClient_->securityMode(loginRequest.securityMode());
		sessionServiceConfig.secureChannelClient_->securityPolicy(loginRequest.securityPolicy());
		sessionServiceConfig.session_->sessionName(sessionName_);
		sessionServiceConfig.ioThreadName(ioThread_->name());
		sessionServiceConfig.sessionMode_ = loginRequest.sessionMode();
		sessionServiceConfig.sessionServiceChangeHandler_ = sessionStateUpdate;
		sessionServiceConfig.sessionServiceChangeHandlerStrand_ = strand_;
		sessionServiceConfig.session_->reconnectTimeout(0);
		sessionServiceConfig.sessionServiceName_ = std::string("SessionService_") + UniqueId::createStringUniqueId();
		switch (loginRequest.userAuthentication().userAuthType())
		{
			case UserAuthType::Anonymous:
			{
				Log(Debug, "anonymous authentication");

				sessionServiceConfig.session_->authenticationAnonymous(
				    loginRequest.userAuthentication().getAttribute("PolicyId")
				);
				break;
			}
			case UserAuthType::UserName:
			{
				Log(Debug, "user name authentication");

				sessionServiceConfig.session_->authenticationUserName(
					loginRequest.userAuthentication().getAttribute("PolicyId"),
					loginRequest.userAuthentication().getAttribute("UserName"),
					loginRequest.userAuthentication().getAttribute("Password"),
					loginRequest.userAuthentication().getAttribute("SecurityPolicyUri")
				);
				break;
			}
			case UserAuthType::X509:
			{
				Log(Debug, "X509 authentication");

				// create certificate
				auto certData = loginRequest.userAuthentication().getAttribute("Certificate");
				Certificate::SPtr certificate = boost::make_shared<Certificate>();
				MemoryBuffer certDataBuf(certData.length()/2);
				hexStringToByteSequence(certData, (uint8_t*)certDataBuf.memBuf());
				if (!certificate->fromDERBuf(certDataBuf)) {
					Log(Error, "create certificate error")
						.parameter("Id", id_);
					return BadInvalidArgument;
				}

				// create private key
				auto privateKeyData = loginRequest.userAuthentication().getAttribute("PrivateKey");
				PrivateKey::SPtr privateKey = boost::make_shared<PrivateKey>();
				MemoryBuffer privateKeyBuf(privateKeyData.length()/2);
					hexStringToByteSequence(privateKeyData, (uint8_t*)privateKeyBuf.memBuf());
					if (!certificate->fromDERBuf(privateKeyBuf)) {
						Log(Error, "create private key error")
							.parameter("Id", id_);
						return BadInvalidArgument;
					}

				sessionServiceConfig.session_->authenticationX509(
					loginRequest.userAuthentication().getAttribute("PolicyId"),
					certificate,
					privateKey,
					loginRequest.userAuthentication().getAttribute("SecurityPolicyUri")
				);
				break;
			}
			case UserAuthType::Issued:
			{
				Log(Debug, "issued authentication");

				sessionServiceConfig.session_->authenticationIssued(
					loginRequest.userAuthentication().getAttribute("PolicyId"),
					loginRequest.userAuthentication().getAttribute("TokenData"),
					loginRequest.userAuthentication().getAttribute("SecurityPolicyUri")
				);
				break;
			}
			default:
			{
				Log(Error, "authentication type unknown")
					.parameter("Id", id_)
					.parameter("UserAuthType", (uint32_t)loginRequest.userAuthentication().userAuthType());
				return BadInvalidArgument;
			}
		}
		sessionServiceConfig.sessionServiceChangeHandler_ =
			[this] (SessionBase& session, SessionServiceStateId sessionState) {
			    sessionState_ = sessionState;

			    if (sessionState != SessionServiceStateId::Established &&
				    sessionState != SessionServiceStateId::Disconnected) {
				    return;
			    }

				if (sessionState == SessionServiceStateId::Established) {
					sessionStatusCallback_("Connect");
				}

				if (sessionState == SessionServiceStateId::Disconnected) {
					if (logoutContext_) {
						logoutComplete();
						return;
					}

					sessionStatusCallback_("Disconnect");
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
		// check if the function is called outside the strand
		if (!strand_->running_in_this_thread()) {
			auto logoutContext = boost::make_shared<LogoutContext>();
			logoutContext->requestBody_ = requestBody;
			logoutContext->logoutResponseCallback_ = logoutResponseCallback;
			strand_->dispatch(
				[this, logoutContext]() {
				    logout(logoutContext->requestBody_, logoutContext->logoutResponseCallback_);
			    }
			);
			return;
		}

		Log(Debug, "receive logout request")
			.parameter("Id", id_);

		boost::property_tree::ptree responseBody;

		// check parameter
		if (logoutResponseCallback_) {
			Log(Error, "logout function already called")
				.parameter("Id", id_);
			logoutResponseCallback(BadInvalidArgument, responseBody);
			return;
		}

		// parse logout request
		LogoutRequest logoutRequest;
		if (!logoutRequest.jsonDecode(requestBody)) {
			Log(Error, "decode logout request error")
				.parameter("Id", id_);
			logoutResponseCallback(BadInvalidArgument, responseBody);
			return;
		}

		// init logout context
		logoutContext_ = boost::make_shared<LogoutContext>();
		logoutContext_->logoutResponseCallback_ = logoutResponseCallback;

		// close the connection to the opc ua server
		if (sessionState_ != SessionServiceStateId::Disconnected) {
		    sessionService_->asyncDisconnect();
		    return;
		}

		// opc ua connection is closed
		logoutComplete();
	}

	void
	Client::logoutComplete(void)
	{
    	// delete services
    	sessionService_.reset();
    	attributeService_.reset();
        methodService_.reset();
        subscriptionService_.reset();
    	monitoredItemService_.reset();

    	// deregister io thread from service set manager
    	serviceSetManager_.deregisterIOThread(ioThread_->name());

    	// call logout complete callback
    	auto logoutResponseCallback = logoutContext_->logoutResponseCallback_;
    	logoutContext_ = nullptr;

    	boost::property_tree::ptree responseBody;
    	logoutResponseCallback(Success, responseBody);
	}

	// ----------------------------------------------------------------------
	// ----------------------------------------------------------------------
	//
	// attribute service
	//
	// ----------------------------------------------------------------------
	// ----------------------------------------------------------------------
	bool
	Client::initAttributeService(const MessageResponseCallback& messageResponseCallback)
	{
		if (!attributeService_) {
			AttributeServiceConfig attributeServiceConfig;
			attributeServiceConfig.ioThreadName(ioThread_->name());
			attributeServiceConfig.attributeServiceName_ = std::string("AttributeService_") + UniqueId::createStringUniqueId();
			attributeService_ = serviceSetManager_.attributeService(sessionService_, attributeServiceConfig);
			if (!attributeService_) {
				Log(Error, "attribute service error")
					.parameter("Id", id_);
				boost::property_tree::ptree responseBody;
				messageResponseCallback(BadResourceUnavailable, responseBody);
				return false;
			}
		}
		return true;
	}

	void
	Client::read(
		const RequestInfo& requestInfo,
		boost::property_tree::ptree& requestBody,
		const MessageResponseCallback& messageResponseCallback
	)
	{
		Log(Debug, "receive read request")
			.parameter("Id", id_);

		// create attribute service if not exist
		if (!initAttributeService(messageResponseCallback)) return;

		// decode read request from web socket
		auto trx = boost::make_shared<ServiceTransactionRead>();
		trx->requestTimeout(requestInfo.requestTimeout());
		auto req = trx->request();
		if (!req->jsonDecode(requestBody)) {
			Log(Error, "decode read request error")
				.parameter("Id", id_);
			boost::property_tree::ptree responseBody;
			messageResponseCallback(BadInvalidArgument, responseBody);
			return;
		}
		if (req->readValueIdArray()->size() == 0) {
			Log(Error, "decode read request error")
				.parameter("Id", id_);
			boost::property_tree::ptree responseBody;
			messageResponseCallback(BadInvalidArgument, responseBody);
			return;
		}

		// send read request to opc ua server
		trx->resultHandler(
			[this, messageResponseCallback](ServiceTransactionRead::SPtr& trx) {
				boost::property_tree::ptree responseBody;

				// check status code
				if (trx->statusCode() != Success) {
					messageResponseCallback(trx->statusCode(), responseBody);
					return;
				}

				// encode request response
				auto res = trx->response();
				if (!res->jsonEncode(responseBody)) {
					messageResponseCallback(BadDeviceFailure, responseBody);
					return;
				}

				messageResponseCallback(Success, responseBody);
			}
		);
		attributeService_->asyncSend(trx);
	}

	void
	Client::write(
		const RequestInfo& requestInfo,
		boost::property_tree::ptree& requestBody,
		const MessageResponseCallback& messageResponseCallback
	)
	{
		Log(Debug, "receive write request")
			.parameter("Id", id_);

		// create attribute service if not exist
		if (!initAttributeService(messageResponseCallback)) return;

		// decode write request from web socket
		auto trx = boost::make_shared<ServiceTransactionWrite>();
		trx->requestTimeout(requestInfo.requestTimeout());
		auto req = trx->request();
		if (!req->jsonDecode(requestBody)) {
			Log(Error, "decode write request error")
				.parameter("Id", id_);
			boost::property_tree::ptree responseBody;
			messageResponseCallback(BadInvalidArgument, responseBody);
			return;
		}
		if (req->writeValueArray()->size() == 0) {
			Log(Error, "decode write request error")
				.parameter("Id", id_);
			boost::property_tree::ptree responseBody;
			messageResponseCallback(BadInvalidArgument, responseBody);
			return;
		}

		// send write request to opc ua server
		trx->resultHandler(
			[this, messageResponseCallback](ServiceTransactionWrite::SPtr& trx) {
				boost::property_tree::ptree responseBody;

				// check status code
				if (trx->statusCode() != Success) {
					messageResponseCallback(trx->statusCode(), responseBody);
					return;
				}

				// encode request response
				auto res = trx->response();
				if (!res->jsonEncode(responseBody)) {
					messageResponseCallback(BadDeviceFailure, responseBody);
					return;
				}

				messageResponseCallback(Success, responseBody);
			}
		);
		attributeService_->asyncSend(trx);
	}

	void
	Client::historyRead(
		const RequestInfo& requestInfo,
		boost::property_tree::ptree& requestBody,
		const MessageResponseCallback& messageResponseCallback
	)
	{
		Log(Debug, "receive history read request")
			.parameter("Id", id_);

		// create attribute service if not exist
		if (!initAttributeService(messageResponseCallback)) return;

		// decode history read request from web socket
		auto trx = boost::make_shared<ServiceTransactionHistoryRead>();
		trx->requestTimeout(requestInfo.requestTimeout());
		auto req = trx->request();
		if (!req->jsonDecode(requestBody)) {
			Log(Error, "decode history read request error")
				.parameter("Id", id_);
			boost::property_tree::ptree responseBody;
			messageResponseCallback(BadInvalidArgument, responseBody);
			return;
		}

		// send write request to opc ua server
		trx->resultHandler(
			[this, messageResponseCallback](ServiceTransactionHistoryRead::SPtr& trx) {
				boost::property_tree::ptree responseBody;

				// check status code
				if (trx->statusCode() != Success) {
					messageResponseCallback(trx->statusCode(), responseBody);
					return;
				}

				// encode request response
				auto res = trx->response();
				if (!res->jsonEncode(responseBody)) {
					messageResponseCallback(BadDeviceFailure, responseBody);
					return;
				}

				messageResponseCallback(Success, responseBody);
			}
		);
		attributeService_->asyncSend(trx);
	}

	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	//
	// view service
	//
	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	bool
	Client::initViewService(const MessageResponseCallback& messageResponseCallback)
	{
		if (!viewService_) {
			ViewServiceConfig viewServiceConfig;
			viewServiceConfig.ioThreadName(ioThread_->name());
			viewServiceConfig.viewServiceName_ = std::string("ViewService_") + UniqueId::createStringUniqueId();
			viewService_ = serviceSetManager_.viewService(sessionService_, viewServiceConfig);
			if (!viewService_) {
				Log(Error, "view service error")
					.parameter("Id", id_);
				boost::property_tree::ptree responseBody;
				messageResponseCallback(BadResourceUnavailable, responseBody);
				return false;
			}
		}
		return true;
	}

	void
	Client::browse(
		const RequestInfo& requestInfo,
		boost::property_tree::ptree& requestBody,
		const MessageResponseCallback& messageResponseCallback
	)
	{
		Log(Debug, "receive browse request")
			.parameter("Id", id_);

		// create attribute service if not exist
		if (!initViewService(messageResponseCallback)) return;

		// decode browse request from web socket
		auto trx = boost::make_shared<ServiceTransactionBrowse>();
		trx->requestTimeout(requestInfo.requestTimeout());
		auto req = trx->request();
		if (!req->jsonDecode(requestBody)) {
			Log(Error, "decode browse request error")
				.parameter("Id", id_);
			boost::property_tree::ptree responseBody;
			messageResponseCallback(BadInvalidArgument, responseBody);
			return;
		}

		// send browse request to opc ua server
		trx->resultHandler(
			[this, messageResponseCallback](ServiceTransactionBrowse::SPtr& trx) {
				boost::property_tree::ptree responseBody;

				// check status code
				if (trx->statusCode() != Success) {
					messageResponseCallback(trx->statusCode(), responseBody);
					return;
				}

				// encode request response
				auto res = trx->response();
				if (!res->jsonEncode(responseBody)) {
					messageResponseCallback(BadDeviceFailure, responseBody);
					return;
				}

				messageResponseCallback(Success, responseBody);
			}
		);
		viewService_->asyncSend(trx);
	}

	void
	Client::browseNext(
		const RequestInfo& requestInfo,
		boost::property_tree::ptree& requestBody,
		const MessageResponseCallback& messageResponseCallback
	)
	{
		Log(Debug, "receive browse next request")
			.parameter("Id", id_);

		// create attribute service if not exist
		if (!initViewService(messageResponseCallback)) return;

		// decode browse request from web socket
		auto trx = boost::make_shared<ServiceTransactionBrowseNext>();
		trx->requestTimeout(requestInfo.requestTimeout());
		auto req = trx->request();
		if (!req->jsonDecode(requestBody)) {
			Log(Error, "decode browse next request error")
				.parameter("Id", id_);
			boost::property_tree::ptree responseBody;
			messageResponseCallback(BadInvalidArgument, responseBody);
			return;
		}

		// send browse request to opc ua server
		trx->resultHandler(
			[this, messageResponseCallback](ServiceTransactionBrowseNext::SPtr& trx) {
				boost::property_tree::ptree responseBody;

				// check status code
				if (trx->statusCode() != Success) {
					messageResponseCallback(trx->statusCode(), responseBody);
					return;
				}

				// encode request response
				auto res = trx->response();
				if (!res->jsonEncode(responseBody)) {
					messageResponseCallback(BadDeviceFailure, responseBody);
					return;
				}

				messageResponseCallback(Success, responseBody);
			}
		);
		viewService_->asyncSend(trx);
	}

	void
	Client::translateBrowsePathsToNodeIds(
		const RequestInfo& requestInfo,
		boost::property_tree::ptree& requestBody,
		const MessageResponseCallback& messageResponseCallback
	)
	{
		Log(Debug, "receive translate browse paths to node id request")
			.parameter("Id", id_);

		// create attribute service if not exist
		if (!initViewService(messageResponseCallback)) return;

		// decode browse request from web socket
		auto trx = boost::make_shared<ServiceTransactionTranslateBrowsePathsToNodeIds>();
		trx->requestTimeout(requestInfo.requestTimeout());
		auto req = trx->request();
		if (!req->jsonDecode(requestBody)) {
			Log(Error, "decode translate browse paths to node ids request error")
				.parameter("Id", id_);
			boost::property_tree::ptree responseBody;
			messageResponseCallback(BadInvalidArgument, responseBody);
			return;
		}

		// send browse request to opc ua server
		trx->resultHandler(
			[this, messageResponseCallback](ServiceTransactionTranslateBrowsePathsToNodeIds::SPtr& trx) {
				boost::property_tree::ptree responseBody;

				// check status code
				if (trx->statusCode() != Success) {
					messageResponseCallback(trx->statusCode(), responseBody);
					return;
				}

				// encode request response
				auto res = trx->response();
				if (!res->jsonEncode(responseBody)) {
					messageResponseCallback(BadDeviceFailure, responseBody);
					return;
				}

				messageResponseCallback(Success, responseBody);
			}
		);
		viewService_->asyncSend(trx);
	}

	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	//
	// method service
	//
	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	bool
	Client::initMethodService(const MessageResponseCallback& messageResponseCallback)
	{
		if (!methodService_) {
			MethodServiceConfig methodServiceConfig;
			methodServiceConfig.ioThreadName(ioThread_->name());
			methodServiceConfig.methodServiceName_ = std::string("MethodService_") + UniqueId::createStringUniqueId();
			methodService_ = serviceSetManager_.methodService(sessionService_, methodServiceConfig);
			if (!methodService_) {
				Log(Error, "method service error")
					.parameter("Id", id_);
				boost::property_tree::ptree responseBody;
				messageResponseCallback(BadResourceUnavailable, responseBody);
				return false;
			}
		}
		return true;
	}

	void
	Client::call(
		const RequestInfo& requestInfo,
		boost::property_tree::ptree& requestBody,
		const MessageResponseCallback& messageResponseCallback
	)
	{
		Log(Debug, "receive call request")
				.parameter("Id", id_);

		// create method service if not exist
		if (!initMethodService(messageResponseCallback)) return;

		// decode method request from web socket
		auto trx = boost::make_shared<ServiceTransactionCall>();
		trx->requestTimeout(requestInfo.requestTimeout());
		auto req = trx->request();
		if (!req->jsonDecode(requestBody)) {
			Log(Error, "decode call request error")
				.parameter("Id", id_);
			boost::property_tree::ptree responseBody;
			messageResponseCallback(BadInvalidArgument, responseBody);
			return;
		}

		// send write request to opc ua server
		trx->resultHandler(
			[this, messageResponseCallback](ServiceTransactionCall::SPtr& trx) {
				boost::property_tree::ptree responseBody;

				// check status code
				if (trx->statusCode() != Success) {
					messageResponseCallback(trx->statusCode(), responseBody);
					return;
				}

				// encode call response
				auto res = trx->response();
				if (!res->jsonEncode(responseBody)) {
					messageResponseCallback(BadDeviceFailure, responseBody);
					return;
				}

				messageResponseCallback(Success, responseBody);
			}
		);
		methodService_->asyncSend(trx);
	}

	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	//
	// subscription service
	//
	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	bool
	Client::initSubscriptionService(const MessageResponseCallback& messageResponseCallback)
	{
		if (!subscriptionService_) {

			auto dataChangeHandler = [this](const MonitoredItemNotification::SPtr& monitoredItem) {
				if (dataChangeCallback_) {
					dataChangeCallback_(
						monitoredItem->clientHandle(),
						monitoredItem->value()
					);
				}
			};

			auto eventHandler = [this](const EventFieldList::SPtr& eventFieldList) {
				if (eventCallback_) {
					eventCallback_(
						eventFieldList->clientHandle(),
						eventFieldList->eventFields()
					);
				}
			};

			auto subscriptionStateHandler = [this](SubscriptionState subscriptionState, uint32_t subscriptionId) {
				std::string state = "Unknown";
				if (subscriptionState == SubscriptionState::SS_Open) {
					state = "Open";
				}
				else if (subscriptionState == SubscriptionState::SS_Close) {
					state = "Close";
				}

				if (subscriptionStatusCallback_) {
					subscriptionStatusCallback_(subscriptionId, state);
				}
			};

			SubscriptionServiceConfig subscriptionServiceConfig;
			subscriptionServiceConfig.ioThreadName(ioThread_->name());
			subscriptionServiceConfig.dataChangeNotificationHandler_ = dataChangeHandler;
			subscriptionServiceConfig.eventNotificationHandler_ = eventHandler;
			subscriptionServiceConfig.subscriptionStateUpdateHandler_ = subscriptionStateHandler;
			subscriptionServiceConfig.subscriptionServiceName_ = std::string("SubscriptionService_") + UniqueId::createStringUniqueId();
			subscriptionService_ = serviceSetManager_.subscriptionService(sessionService_, subscriptionServiceConfig);
			if (!subscriptionService_) {
				Log(Error, "subscription service error")
					.parameter("Id", id_);
				boost::property_tree::ptree responseBody;
				messageResponseCallback(BadResourceUnavailable, responseBody);
				return false;
			}
		}
		return true;
	}

	void
	Client::subscriptionStatusCallback(
	    const SubscriptionStatusCallback& subscriptionStatusCallback
	)
	{
		subscriptionStatusCallback_ = subscriptionStatusCallback;
	}

	void
	Client::dataChangeCallback(
		const DataChangeCallback dataChangeCallback
	)
	{
		dataChangeCallback_ = dataChangeCallback;
	}

	void
	Client::eventCallback(
		const EventCallback eventCallback
	)
	{
		eventCallback_ = eventCallback;
	}

	void
	Client::createSubscription(
		const RequestInfo& requestInfo,
		boost::property_tree::ptree& requestBody,
		const MessageResponseCallback& messageResponseCallback
	)
	{
		Log(Debug, "receive create subscription request")
				.parameter("Id", id_);

		// create subscription service if not exist
		if (!initSubscriptionService(messageResponseCallback)) return;

		// decode create subscription request from web socket
		auto trx = boost::make_shared<ServiceTransactionCreateSubscription>();
		trx->requestTimeout(requestInfo.requestTimeout());
		auto req = trx->request();
		if (!req->jsonDecode(requestBody)) {
			Log(Error, "decode create subscription request error")
				.parameter("Id", id_);
			boost::property_tree::ptree responseBody;
			messageResponseCallback(BadInvalidArgument, responseBody);
			return;
		}

		// send create subscription request to opc ua server
		trx->resultHandler(
			[this, messageResponseCallback](ServiceTransactionCreateSubscription::SPtr& trx) {
				boost::property_tree::ptree responseBody;

				// check status code
				if (trx->statusCode() != Success) {
					messageResponseCallback(trx->statusCode(), responseBody);
					return;
				}

				// encode call response
				auto res = trx->response();
				if (!res->jsonEncode(responseBody)) {
					messageResponseCallback(BadDeviceFailure, responseBody);
					return;
				}

				messageResponseCallback(Success, responseBody);
			}
		);
		subscriptionService_->asyncSend(trx);
	}

	void
	Client::deleteSubscriptions(
		const RequestInfo& requestInfo,
		boost::property_tree::ptree& requestBody,
		const MessageResponseCallback& messageResponseCallback
	)
	{
		Log(Debug, "receive delete subscriptions request")
				.parameter("Id", id_);

		// create subscription service if not exist
		if (!initSubscriptionService(messageResponseCallback)) return;

		// decode delete subscriptions request from web socket
		auto trx = boost::make_shared<ServiceTransactionDeleteSubscriptions>();
		trx->requestTimeout(requestInfo.requestTimeout());
		auto req = trx->request();
		if (!req->jsonDecode(requestBody)) {
			Log(Error, "decode delete subscriptions request error")
				.parameter("Id", id_);
			boost::property_tree::ptree responseBody;
			messageResponseCallback(BadInvalidArgument, responseBody);
			return;
		}

		// send delete subscriptions request to opc ua server
		trx->resultHandler(
			[this, messageResponseCallback](ServiceTransactionDeleteSubscriptions::SPtr& trx) {
				boost::property_tree::ptree responseBody;

				// check status code
				if (trx->statusCode() != Success) {
					messageResponseCallback(trx->statusCode(), responseBody);
					return;
				}

				// encode call response
				auto res = trx->response();
				if (!res->jsonEncode(responseBody)) {
					messageResponseCallback(BadDeviceFailure, responseBody);
					return;
				}

				messageResponseCallback(Success, responseBody);
			}
		);
		subscriptionService_->asyncSend(trx);
	}


	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	//
	// monitored item service
	//
	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	bool
	Client::initMonitoredItemService(const MessageResponseCallback& messageResponseCallback)
	{
		if (!monitoredItemService_) {
			MonitoredItemServiceConfig monitoredItemServiceConfig;
			monitoredItemServiceConfig.ioThreadName(ioThread_->name());
			monitoredItemServiceConfig.monitoredItemServiceName_ = std::string("MonitoredItemService_") + UniqueId::createStringUniqueId();
			monitoredItemService_ = serviceSetManager_.monitoredItemService(sessionService_, monitoredItemServiceConfig);
			if (!monitoredItemService_) {
				Log(Error, "monitored item service error")
					.parameter("Id", id_);
				boost::property_tree::ptree responseBody;
				messageResponseCallback(BadResourceUnavailable, responseBody);
				return false;
			}
		}
		return true;
	}

	void
	Client::createMonitoredItems(
		const RequestInfo& requestInfo,
		boost::property_tree::ptree& requestBody,
		const MessageResponseCallback& messageResponseCallback
	)
	{
		Log(Debug, "receive create monitored items request")
				.parameter("Id", id_);

		// create monitored item service if not exist
		if (!initMonitoredItemService(messageResponseCallback)) return;

		// decode create monitored items request from web socket
		auto trx = boost::make_shared<ServiceTransactionCreateMonitoredItems>();
		trx->requestTimeout(requestInfo.requestTimeout());
		auto req = trx->request();
		if (!req->jsonDecode(requestBody)) {
			Log(Error, "decode create monitored items request error")
				.parameter("Id", id_);
			boost::property_tree::ptree responseBody;
			messageResponseCallback(BadInvalidArgument, responseBody);
			return;
		}

		// send create monitored items request to opc ua server
		trx->resultHandler(
			[this, messageResponseCallback](ServiceTransactionCreateMonitoredItems::SPtr& trx) {
				boost::property_tree::ptree responseBody;

				// check status code
				if (trx->statusCode() != Success) {
					messageResponseCallback(trx->statusCode(), responseBody);
					return;
				}

				// encode call response
				auto res = trx->response();
				if (!res->jsonEncode(responseBody)) {
					messageResponseCallback(BadDeviceFailure, responseBody);
					return;
				}

				messageResponseCallback(Success, responseBody);
			}
		);
		monitoredItemService_->asyncSend(trx);
	}

	void
	Client::deleteMonitoredItems(
		const RequestInfo& requestInfo,
		boost::property_tree::ptree& requestBody,
		const MessageResponseCallback& messageResponseCallback
	)
	{
		Log(Debug, "receive delete monitored items request")
				.parameter("Id", id_);

		// create monitored item service if not exist
		if (!initMonitoredItemService(messageResponseCallback)) return;

		// decode delete monitored itmes request from web socket
		auto trx = boost::make_shared<ServiceTransactionDeleteMonitoredItems>();
		trx->requestTimeout(requestInfo.requestTimeout());
		auto req = trx->request();
		if (!req->jsonDecode(requestBody)) {
			Log(Error, "decode delete monitored items request error")
				.parameter("Id", id_);
			boost::property_tree::ptree responseBody;
			messageResponseCallback(BadInvalidArgument, responseBody);
			return;
		}

		// send delete monitored items request to opc ua server
		trx->resultHandler(
			[this, messageResponseCallback](ServiceTransactionDeleteMonitoredItems::SPtr& trx) {
				boost::property_tree::ptree responseBody;

				// check status code
				if (trx->statusCode() != Success) {
					messageResponseCallback(trx->statusCode(), responseBody);
					return;
				}

				// encode call response
				auto res = trx->response();
				if (!res->jsonEncode(responseBody)) {
					messageResponseCallback(BadDeviceFailure, responseBody);
					return;
				}

				messageResponseCallback(Success, responseBody);
			}
		);
		monitoredItemService_->asyncSend(trx);
	}

}
