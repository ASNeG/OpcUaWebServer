/*
   Copyright 2015-2019 Kai Huebl (kai@huebl-sgh.de)

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
#include "OpcUaWebServer/WebSocket/WebSocket.h"

using boost::lexical_cast;
using boost::bad_lexical_cast;

namespace OpcUaWebServer
{


	WebSocket::WebSocket(void)
	{
	}

	WebSocket::~WebSocket(void)
	{
	}

	void
	WebSocket::startup(
		Config* config,
		const IOThread::SPtr& ioThread,
		const WebSocketServer::ReceiveMessageCallback& receiveMessageCallback,
		const StartupCompleteCallback& startupCompleteCallback
	)
	{
		strand_ = ioThread->createStrand();
		strand_->post(
			[this, config, ioThread, receiveMessageCallback, startupCompleteCallback]() {
				startupStrand(config, ioThread, receiveMessageCallback, startupCompleteCallback);
			}
		);
	}

	void
	WebSocket::startupStrand(
		Config* config,
		const IOThread::SPtr& ioThread,
		const WebSocketServer::ReceiveMessageCallback& receiveMessageCallback,
		const StartupCompleteCallback& startupCompleteCallback
	)
	{
		startupCompleteCallback_ = startupCompleteCallback;
		webSocketConfig_.ioThread(ioThread);

        if (!getWebSocketConfig(config)) {
	        startupCompleteCallback_(false);
	        return;
        }

        if (!webSocketConfig_.enable()) {
	        startupCompleteCallback_(true);
	        return;
        }

        webSocketServer_ = constructSPtr<WebSocketServer>(&webSocketConfig_);
        webSocketServer_->receiveMessageCallback(receiveMessageCallback);
        if (!webSocketServer_->startup()) {
	         startupCompleteCallback_(false);
	         return;
        }

        startupCompleteCallback_(true);
        return;
	}

	bool
	WebSocket::shutdown(
		void
	)
	{
		if (!webSocketConfig_.enable()) {
			return true;
		}

		return true;
	}

	bool
	WebSocket::getWebSocketConfig(Config* config)
	{
		bool success;

		// check enable flag
		if (
			!config->exist("OpcUaWebServerModel.WebSocketServer") ||
			config->exist("OpcUaWebServerModel.WebSocketServer.<xmlattr>.Disable")
		) {
			webSocketConfig_.enable(false);
			return true;
		}

		// read ip address
		std::string address;
		success = config->getConfigParameter("OpcUaWebServerModel.WebSocketServer.Address", address);
		if (!success) {
			Log(Error, "missing web socket parameter in configuration file")
				.parameter("Parameter", "OpcUaWebServerModel.WebSocketServer.Address")
				.parameter("ConfigurationFile", config->configFileName());
			return false;
		}
		webSocketConfig_.address(address);

		// read port
		std::string portString;
		success = config->getConfigParameter("OpcUaWebServerModel.WebSocketServer.Port", portString);
		if (!success) {
			Log(Error, "missing web socket parameter in configuration file")
				.parameter("Parameter", "OpcUaWebServerModel.WebSocketServer.Port")
				.parameter("ConfigurationFile", config->configFileName());
			return false;
		}

		uint32_t port;
		try {
			port = lexical_cast<uint32_t>(portString);
		}
		catch (bad_lexical_cast &) {
			Log(Error, "invalid web socket parameter in configuration file")
				.parameter("Port", portString)
				.parameter("Parameter", "OpcUaWebServerModel.WebSocketServer.Port")
				.parameter("ConfigurationFileName", config->configFileName());
			return false;
		}
		webSocketConfig_.port(port);

		// read web request timeout
		uint32_t requestTimeout;
		success = config->getConfigParameter("OpcUaWebServerModel.WebSocketServer.RequestTimeout", requestTimeout);
		if (!success) {
			Log(Error, "missing web server parameter in configuration file")
				.parameter("Parameter", "OpcUaWebServerModel.WebSocketServer.RequestTimeout")
				.parameter("ConfigurationFile", config->configFileName());
			return false;
		}
		webSocketConfig_.requestTimeout(requestTimeout);

		// read idle timeout
		uint32_t idleTimeout;
		success = config->getConfigParameter("OpcUaWebServerModel.WebSocketServer.IdleTimeout", idleTimeout);
		if (!success) {
			Log(Error, "missing web server parameter in configuration file")
				.parameter("Parameter", "OpcUaWebServerModel.WebSocketServer.IdleTimeout")
				.parameter("ConfigurationFile", config->configFileName());
			return false;
		}
		webSocketConfig_.idleTimeout(idleTimeout);

		return true;
	}

	bool
	WebSocket::sendMessage(
		WebSocketMessage& webSocketMessage
	)
	{
		return webSocketServer_->sendMessage(webSocketMessage);
	}

}
