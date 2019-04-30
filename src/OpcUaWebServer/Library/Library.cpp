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

#include "OpcUaStackCore/Base/os.h"
#include "OpcUaStackCore/Base/Log.h"
#include "OpcUaStackCore/Base/ConfigXml.h"
#include "OpcUaWebServer/Library/Library.h"
#include "OpcUaStackServer/ServiceSetApplication/ApplicationService.h"
#include "OpcUaStackServer/ServiceSetApplication/NodeReferenceApplication.h"
#include <iostream>
#include "BuildConfig.h"

namespace OpcUaWebServer
{

	Library::Library(void)
	: ApplicationIf()
	, webServer_()
	, webSocket_()
	, messageServer_()
	, opcUaClientManager_()
	, ioThread_()
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

		ioThread_ = constructSPtr<IOThread>();
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
		if (!webServer_.startup(&config, ioThread_)) return false;
		Log(Info, "startup web server");

		rc = webSocket_.startup(
			&config,
			ioThread_,
			[this](WebSocketMessage& webSocketMessage) {
				std::cout << "WebSocketMessage: " << webSocketMessage.message_ << std::endl;
				messageServer_.receiveMessage(webSocketMessage.channelId_, webSocketMessage.message_);
			}
		);
		Log(Info, "startup web socket");

		if (!rc) return false;
		rc = webGateway_.startup(
			&config, ioThread_,
			cryptoManager()
		);
		if (!rc) return false;
		Log(Info, "startup web gateway");

		if (!messageServer_.startup(&config, this)) return false;
		Log(Info, "startup message server");

		if (!opcUaClientManager_.startup(&config, this, ioThread_, cryptoManager())) return false;
		Log(Info, "startup opc ua client manager");

		return true;
	}

	bool
	Library::shutdown(void)
	{
		Log(Debug, "Library::shutdown");

		if (!ioThread_->shutdown()) return false;
		ioThread_.reset();
		Log(Info, "shutdown io thread");

		if (!opcUaClientManager_.shutdown()) return false;
		Log(Info, "shutdown opc ua client manager");

		if (!messageServer_.shutdown()) return false;
		Log(Info, "shutdown message server");

		if (!webGateway_.shutdown()) return false;
		Log(Info, "shutdown web gateway");

		if (!webSocket_.shutdown()) return false;
		Log(Info, "shutdown web socket");

		if (!webServer_.shutdown()) return false;
		Log(Info, "shutdown web server");

		return true;
	}

	std::string
	Library::version(void)
	{
		std::stringstream version;

		version << LIBRARY_VERSION_MAJOR << "." << LIBRARY_VERSION_MINOR << "." << LIBRARY_VERSION_PATCH;
		return version.str();
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
		WebSocketMessage webSocketMessage;
		webSocketMessage.channelId_ = channelId;
		webSocketMessage.message_ = message;
		webSocket_.sendMessage(webSocketMessage);
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

