/*
   Copyright 2015-2020 Kai Huebl (kai@huebl-sgh.de)

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

#include <future>
#include <boost/make_shared.hpp>
#include "OpcUaStackCore/Base/os.h"
#include "OpcUaStackCore/Base/Log.h"
#include "OpcUaStackCore/Base/ConfigXml.h"
#include "OpcUaWebServer/Library/Library.h"
#include "OpcUaStackServer/ServiceSetApplication/ApplicationService.h"
#include "OpcUaStackServer/ServiceSetApplication/NodeReferenceApplication.h"
#include <iostream>
#include "BuildConfig.h"

using namespace OpcUaStackCore;
using namespace OpcUaStackServer;

namespace OpcUaWebServer
{

	Library::Library(void)
	: ApplicationIf()
	, webServer_()
	, webSocket_()
	, messageServer_()
	, opcUaClientManager_()
	{
	}

	Library::~Library(void)
	{
	}

	bool
	Library::startup(void)
	{
		bool rc;
		Log(Debug, "Library::startup");

		// create message bus
		messageBus_ = boost::shared_ptr<MessageBus>();

		// create thread pool
		ioThread_ = boost::make_shared<IOThread>();
		ioThread_->name(std::string("ThrWeb"));
		ioThread_->numberThreads(2);
		if (!ioThread_->startup()) {
			return false;
		}

		// read web socket configuration file
		ConfigXml configXml;
		Config config;
		if (!configXml.parse(applicationInfo()->configFileName(), &config)) {
			Log(Error, "parse configuration file error")
				.parameter("ConfigFileName", applicationInfo()->configFileName())
				.parameter("Reason", configXml.errorMessage());
			return false;
		}

		// start web server components
		if (!startupWebServer(config)) {
			return false;
		}

		//
		// startup websocket
		//
		if (!startupWebSocket(config)) {
			return false;
		}

		//
		// startup web gateway
		//
		if (!startupWebGateway(config)) {
			return false;
		}

		Log(Info, "startup message server");
		if (!messageServer_.startup(&config, this)) {
			return false;
		}

		Log(Info, "startup opc ua client manager");
		if (!opcUaClientManager_.startup(&config, this, ioThread_, messageBus_, cryptoManager())) {
			return false;
		}

		return true;
	}

	bool
	Library::startupWebServer(Config& config)
	{
		Log(Info, "startup http server");

		std::promise<bool> prom;
		auto future = prom.get_future();
		auto startupCompleteCallback = [&prom](bool error) {
			prom.set_value(error);
		};
		webServer_.startup(
			&config,
			ioThread_,
			startupCompleteCallback
		);
		future.wait();
		if (!future.get()) {
			return false;
		}

		return true;
	}

	bool
	Library::startupWebSocket(Config& config)
	{
		Log(Info, "startup web socket");

		std::promise<bool> prom;
		auto future = prom.get_future();
		auto startupCompleteCallback = [&prom](bool error) {
			prom.set_value(error);
		};
		auto receiveMessageCallback = [this](WebSocketMessage::SPtr& webSocketMessage) {
			std::cout << "WebSocketMessage: " << webSocketMessage->message_ << std::endl;
			messageServer_.receiveMessage(webSocketMessage->channelId_, webSocketMessage->message_);
		};
		webSocket_.startup(
			&config,
			ioThread_,
			receiveMessageCallback,
			startupCompleteCallback
		);
		future.wait();
		if (!future.get()) {
			return false;
		}

		return true;
	}

	bool
	Library::startupWebGateway(Config& config)
	{
		Log(Info, "startup web gateway");

		std::promise<bool> prom;
		auto future = prom.get_future();
		auto startupCompleteCallback = [&prom](bool error) {
			prom.set_value(error);
		};
		webGateway_.startup(
			&config,
			ioThread_,
			messageBus_,
			cryptoManager(),
			startupCompleteCallback
		);
		future.wait();
		if (!future.get()) {
			return false;
		}

		return true;
	}

	bool
	Library::shutdown(void)
	{
		Log(Debug, "Library::shutdown");

		Log(Info, "shutdown opc ua client manager");
		if (!opcUaClientManager_.shutdown()) return false;

		Log(Info, "shutdown message server");
		if (!messageServer_.shutdown()) return false;

		if (!shutdownWebGateway()) return false;
		if (!shutdownWebSocket()) return false;
		if (!shutdownWebServer()) return false;

		Log(Info, "shutdown io thread");
		if (!ioThread_->shutdown()) return false;
		ioThread_.reset();

		return true;
	}

	bool
	Library::shutdownWebServer(void)
	{
		Log(Info, "shutdown web server");

		std::promise<bool> prom;
		auto future = prom.get_future();
		webServer_.shutdown(
			[&prom] (bool error) {
				prom.set_value(error);
			}
		);

		future.wait();
		if (!future.get()) {
			return false;
		}

		return true;
	}

	bool
	Library::shutdownWebSocket(void)
	{
		Log(Info, "shutdown web socket");

		std::promise<bool> prom;
		auto future = prom.get_future();
		webSocket_.shutdown(
			[&prom] (bool error) {
				prom.set_value(error);
			}
		);

		future.wait();
		if (!future.get()) {
			return false;
		}

		return true;
	}

	bool
	Library::shutdownWebGateway(void)
	{
		Log(Info, "shutdown web gateway");

		std::promise<bool> prom;
		auto future = prom.get_future();
		webGateway_.shutdown(
			[&prom](bool error) {
				prom.set_value(error);
			}
		);

		future.wait();
		if (!future.get()) {
			return false;
		}

		return true;
	}

	std::string
	Library::version(void)
	{
		std::stringstream version;

		version << LIBRARY_VERSION_MAJOR << "." << LIBRARY_VERSION_MINOR << "." << LIBRARY_VERSION_PATCH;
		return version.str();
	}

	std::string
	Library::gitCommit(void)
	{
		return LIBRARY_GIT_COMMIT;
	}

	std::string
	Library::gitBranch(void)
	{
		return LIBRARY_GIT_BRANCH;
	}

	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	//
	// MessageServerIf
	//
	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	void
	Library::messageServerMessage(Message::SPtr& message)
	{
		opcUaClientManager_.receiveMessage(message);
	}

	void
	Library::messageServerMessage(uint32_t channelId, const std::string& message)
	{
		auto webSocketMessage = boost::make_shared<WebSocketMessage>();
		webSocketMessage->channelId_ = channelId;
		webSocketMessage->message_ = message;
		webSocket_.sendMessage(
			webSocketMessage,
			[](bool error) {}
		);
	}

	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	//
	// ClientManagerIf
	//
	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	void
	Library::clientManagerMessage(Message::SPtr& message)
	{
		messageServer_.receiveMessage(message);
	}

}

extern "C" DLLEXPORT void  init(ApplicationIf** applicationIf) {
    *applicationIf = new OpcUaWebServer::Library();
}

