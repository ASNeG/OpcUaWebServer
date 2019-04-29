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

	bool
	WebGateway::startup(
		Config* config,
		IOThread::SPtr ioThread,
		CryptoManager::SPtr& cryptoManager
	)
	{
		//
		// get gateway configuration from configuration file
		//
		if (!getWebGatewayConfig(config)) {
			return false;
		}

		//
		// startup web socket server instance
		//
		webSocketConfig_.address(webGatewayConfig_.address());
		webSocketConfig_.port(webGatewayConfig_.port());
		webSocketConfig_.ioThread(ioThread);

		auto receiveMessageCallback = [this](WebSocketMessage& webSocketMessage) {
			clientManager_.receiveMessage(webSocketMessage);
		};

		webSocketServer_ = constructSPtr<WebSocketServer>(&webSocketConfig_);
		webSocketServer_->receiveMessageCallback(receiveMessageCallback);
		if (!webSocketServer_->startup()) {
			return false;
		}

		//
		// startup client manager
		//
		auto sendMessageCallback = [this](WebSocketMessage& webSocketMessage) {
			webSocketServer_->sendMessage(webSocketMessage);
		};

		clientManager_.sendMessageCallback(sendMessageCallback);
		if (!clientManager_.startup(ioThread, cryptoManager)) {
			return false;
		}

		return true;
	}

	bool
	WebGateway::shutdown(void)
	{

		// shutdown client manager
		clientManager_.shutdown();

		// shutdown web socket server
		if (webSocketServer_) {
			webSocketServer_->shutdown();
		}

		return true;
	}

	bool
	WebGateway::getWebGatewayConfig(Config* config)
	{
		bool success;

		// check if gateway configuration is exist in configuration file. If not, then
		// the web gateway is diabled.
		if (!config->exist("OpcUaWebServerModel.WebGateway")) {
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

		return true;
	}

}

