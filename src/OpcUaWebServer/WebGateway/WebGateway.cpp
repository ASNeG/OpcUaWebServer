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

#include <boost/algorithm/string/case_conv.hpp>
#include <boost/lexical_cast.hpp>
#include "OpcUaStackCore/Base/Log.h"
#include "OpcUaWebServer/WebGateway/WebGateway.h"

using namespace OpcUaStackCore;

namespace OpcUaWebServer
{

	WebGateway::WebGateway(void)
	: webGatewayConfig_()

	, webSocketConfig_()
	, webSocketServer_()

	, clientManager_()
	{
	}

	WebGateway::~WebGateway(void)
	{
	}

	void
	WebGateway::startup(
		Config* config,
		IOThread::SPtr ioThread,
		MessageBus::SPtr messageBus,
		CryptoManager::SPtr cryptoManager,
		const StartupCompleteCallback& startupCompleteCallback
	)
	{
		StartupCompleteCallback tmpStartupCompleteCallback = startupCompleteCallback;
		strand_ = ioThread->createStrand();
		strand_->post(
			[this, config, ioThread, messageBus, cryptoManager, tmpStartupCompleteCallback]() {
				startupStrand(config, ioThread, messageBus, cryptoManager, tmpStartupCompleteCallback);
			}
		);
	}

	void
	WebGateway::startupStrand(
		Config* config,
		IOThread::SPtr ioThread,
		MessageBus::SPtr messageBus,
		CryptoManager::SPtr cryptoManager,
		const StartupCompleteCallback& startupCompleteCallback
	)
	{
		//
		// get gateway configuration from configuration file
		//
		if (!getWebGatewayConfig(config)) {
			startupCompleteCallback(false);
			return;
		}

		if (!webGatewayConfig_.active()) {
			startupCompleteCallback(true);
			return;
		}

		//
		// startup client manager
		//
		Log(Debug, "client manager startup");
		auto sendMessageCallback = [this](WebSocketMessage::SPtr& webSocketMessage) {
			webSocketServer_->sendMessage(webSocketMessage, [](bool error){});
		};

		auto disconnectChannelCallback = [this](uint32_t channelId) {
			webSocketServer_->disconnect(channelId);
		};

		clientManager_.sendMessageCallback(sendMessageCallback);
		clientManager_.disconnectChannelCallback(disconnectChannelCallback);
		if (!clientManager_.startup(ioThread, messageBus, cryptoManager)) {
			startupCompleteCallback(false);
			return;
		}

		//
		// startup web socket server instance
		//
		Log(Debug, "web socket server startup");
		webSocketConfig_.address(webGatewayConfig_.address());
		webSocketConfig_.port(webGatewayConfig_.port());
		webSocketConfig_.ioThread(ioThread);
		webSocketConfig_.maxConnections(webGatewayConfig_.maxConnections());
		webSocketConfig_.maxPacketLength(webGatewayConfig_.maxPacketLength());
		webSocketConfig_.ssl(webGatewayConfig_.ssl());
		webSocketConfig_.csrFile(webGatewayConfig_.csrFile());
		webSocketConfig_.keyFile(webGatewayConfig_.keyFile());

		auto receiveMessageCallback = [this](WebSocketMessage::SPtr& webSocketMessage) {
			clientManager_.receiveMessage(webSocketMessage);
		};

		webSocketServer_ = boost::make_shared<WebSocketServer>(&webSocketConfig_);
		webSocketServer_->receiveMessageCallback(receiveMessageCallback);
        webSocketServer_->startup(
        	[this, startupCompleteCallback](bool error) {
        		startupCompleteCallback(error);
        	}
        );
	}

	void
	WebGateway::shutdown(
		const ShutdownCompleteCallback& shutdownCompleteCallback
	)
	{
		strand_->post(
			[this, shutdownCompleteCallback]() {
				shutdownStrand(shutdownCompleteCallback);
			}
		);
	}

	void
	WebGateway::shutdownStrand(
		const ShutdownCompleteCallback& shutdownCompleteCallback
	)
	{
		if (!webGatewayConfig_.active()) {
			shutdownCompleteCallback(true);
			return;
		}

		if (!webSocketConfig_.enable()) {
			shutdownCompleteCallback(true);
			return;
		}

		// shutdown web socket server
		Log(Debug, "web socket server shutdown");
        webSocketServer_->shutdown(
        	[this, shutdownCompleteCallback](bool error) {

        	    // shutdown client manager
    		    Log(Debug, "client manager shutdown");
    		    clientManager_.shutdown();

        		shutdownCompleteCallback(true);
        	}
        );
	}

	bool
	WebGateway::getWebGatewayConfig(Config* config)
	{
		bool success;

		// check if gateway configuration is exist in configuration file. If not, then
		// the web gateway is diabled.
		if (!config->exist("OpcUaWebServerModel.WebGateway") ||
			 config->exist("OpcUaWebServerModel.WebGateway.<xmlattr>.Disable")
		) {
			Log(Info, "web gateway is disabled");
			webGatewayConfig_.active(false);
			return true;
		}
		webGatewayConfig_.active(true);

		// read ip address
		std::string address;
		success = config->getConfigParameter("OpcUaWebServerModel.WebGateway.Address", address);
		if (!success) {
			Log(Error, "missing web gateway parameter in configuration file")
				.parameter("Parameter", "OpcUaWebServerModel.WebGateway.Address")
				.parameter("ConfigurationFile", config->configFileName());
			return false;
		}
		webGatewayConfig_.address(address);

		// read port
		std::string portString;
		success = config->getConfigParameter("OpcUaWebServerModel.WebGateway.Port", portString);
		if (!success) {
			Log(Error, "missing web gateway parameter in configuration file")
				.parameter("Parameter", "OpcUaWebServerModel.WebGateway.Port")
				.parameter("ConfigurationFile", config->configFileName());
			return false;
		}

		uint32_t port;
		try {
			port = boost::lexical_cast<uint32_t>(portString);
		}
		catch (boost::bad_lexical_cast &) {
			Log(Error, "invalid web socket parameter in configuration file")
				.parameter("Port", portString)
				.parameter("Parameter", "OpcUaWebServerModel.WebSocketServer.Port")
				.parameter("ConfigurationFileName", config->configFileName());
			return false;
		}
		webGatewayConfig_.port(port);

		// read max connections
		std::string maxConnectionsString;
		success = config->getConfigParameter("OpcUaWebServerModel.WebGateway.MaxConnections", maxConnectionsString);
		if (!success) {
			Log(Error, "missing web gateway parameter in configuration file")
				.parameter("Parameter", "OpcUaWebServerModel.WebGateway.MaxConnections")
				.parameter("ConfigurationFile", config->configFileName());
			return false;
		}

		uint32_t maxConnections;
		try {
			maxConnections = boost::lexical_cast<uint32_t>(maxConnectionsString);
		}
		catch (boost::bad_lexical_cast &) {
			Log(Error, "invalid web socket parameter in configuration file")
				.parameter("Port", portString)
				.parameter("Parameter", "OpcUaWebServerModel.WebSocketServer.MaxConnections")
				.parameter("ConfigurationFileName", config->configFileName());
			return false;
		}
		webGatewayConfig_.maxConnections(maxConnections);

		//
		// now we read the optional ssl attributes
		//

		// read ssl element
		std::string sslString;
		success = config->getConfigParameter("OpcUaWebServerModel.WebGateway.SSL", sslString);
		if (success && boost::to_upper_copy(sslString) == "ON") {
			webGatewayConfig_.ssl(true);
		}

		// read certificate file
		if (webGatewayConfig_.ssl()) {
			std::string csrFile;
			success = config->getConfigParameter("OpcUaWebServerModel.WebGateway.CSRFile", csrFile);
			if (!success) {
				Log(Error, "missing web gateway parameter in configuration file")
					.parameter("Parameter", "OpcUaWebServerModel.WebGateway.CSRFile")
					.parameter("ConfigurationFile", config->configFileName());
				return false;
			}
			webGatewayConfig_.csrFile(csrFile);
		}

		// read private key
		if (webGatewayConfig_.ssl()) {
			std::string keyFile;
			success = config->getConfigParameter("OpcUaWebServerModel.WebGateway.KeyFile", keyFile);
			if (!success) {
				Log(Error, "missing web gateway parameter in configuration file")
					.parameter("Parameter", "OpcUaWebServerModel.WebGateway.KeyFile")
					.parameter("ConfigurationFile", config->configFileName());
				return false;
			}
			webGatewayConfig_.keyFile(keyFile);
		}

		return true;
	}

}

