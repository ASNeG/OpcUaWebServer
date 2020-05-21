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

#include <boost/make_shared.hpp>
#include <boost/lexical_cast.hpp>
#include "OpcUaStackCore/Base/Log.h"
#include "OpcUaWebServer/WebServer/WebServer.h"

using boost::lexical_cast;
using boost::bad_lexical_cast;

using namespace OpcUaStackCore;

namespace OpcUaWebServer
{

	WebServer::WebServer(void)
	: httpServer_()
	, httpContent_()
	, httpConfig_()
	{
	}

	WebServer::~WebServer(void)
	{
	}

	void
	WebServer::startup(
		Config* config,
		const IOThread::SPtr& ioThread,
		const StartupCompleteCallback& startupCompleteCallback
	)
	{
		if (!getHttpConfig(config)) {
			startupCompleteCallback(false);
			return;
		}

		if (!httpConfig_.enable()) {
			startupCompleteCallback(true);
			return;
		}

		if (!getIPLoggerConfig(config)) {
			startupCompleteCallback(false);
			return;
		}

		httpConfig_.ioThread(ioThread);
		httpConfig_.strand(ioThread->createStrand());

		// startup http content reader
		httpContent_ = boost::make_shared<HttpContent>();
		httpContent_->httpConfig(&httpConfig_);
		if (!httpContent_->startup()) {
			startupCompleteCallback(false);
			return;
		}

		// startup http server
		httpServer_ = boost::make_shared<HttpServer>(&httpConfig_);
		httpServer_->addHttpServerIf("GET", httpContent_.get());
		httpServer_->startup(
			[this, startupCompleteCallback](bool error) {
				startupCompleteCallback(error);
			}
		);
	}

	void
	WebServer::shutdown(
		const ShutdownCompleteCallback& shutdownCompleteCallback
	)
	{
		if (!httpConfig_.enable()) {
			shutdownCompleteCallback(true);
			return;
		}

		shutdownCompleteCallback_ = shutdownCompleteCallback;
		httpServer_->shutdown([this](bool error) {
			shutdownComplete(error);
		});
	}

	void
	WebServer::shutdownComplete(bool error)
	{
		httpContent_->shutdown();
		httpServer_.reset();
		httpContent_.reset();
		shutdownCompleteCallback_(error);
	}

	bool
	WebServer::getHttpConfig(Config* config)
	{
		bool success;

		// get Disable flag
		if (
			!config->exist("OpcUaWebServerModel.HttpServer") ||
			config->exist("OpcUaWebServerModel.HttpServer.<xmlattr>.Disable")
		) {
			httpConfig_.enable(false);
			return true;
		}

		// read ip address
		std::string address;
		success = config->getConfigParameter("OpcUaWebServerModel.HttpServer.Address", address);
		if (!success) {
			Log(Error, "missing web server parameter in configuration file")
				.parameter("Parameter", "OpcUaWebServerModel.HttpServer.Address")
				.parameter("ConfigurationFile", config->configFileName());
			return false;
		}
		httpConfig_.address(address);

		// read port
		std::string portString;
		success = config->getConfigParameter("OpcUaWebServerModel.HttpServer.Port", portString);
		if (!success) {
			Log(Error, "missing web server parameter in configuration file")
				.parameter("Parameter", "OpcUaWebServerModel.HttpServer.Port")
				.parameter("ConfigurationFile", config->configFileName());
			return false;
		}

		uint32_t port;
		try {
			port = lexical_cast<uint32_t>(portString);
		}
		catch (bad_lexical_cast &) {
			Log(Error, "invalid web server parameter in configuration file")
				.parameter("Port", portString)
				.parameter("Parameter", "OpcUaWebServerModel.HttpServer.Port")
				.parameter("ConfigurationFileName", config->configFileName());
			return false;
		}
		httpConfig_.port(port);

		// read web directory
		std::string webDirectory;
		success = config->getConfigParameter("OpcUaWebServerModel.HttpServer.WebDirectory", webDirectory);
		if (!success) {
			Log(Error, "missing web server parameter in configuration file")
				.parameter("Parameter", "OpcUaWebServerModel.HttpServer.WebDirectory")
				.parameter("ConfigurationFile", config->configFileName());
			return false;
		}
		httpConfig_.webDirectory(webDirectory);

		// read web request timeout
		uint32_t requestTimeout;
		success = config->getConfigParameter("OpcUaWebServerModel.HttpServer.RequestTimeout", requestTimeout);
		if (!success) {
			Log(Error, "missing web server parameter in configuration file")
				.parameter("Parameter", "OpcUaServerModel.OpcUaWebServer.HttpServer.RequestTimeout")
				.parameter("ConfigurationFile", config->configFileName());
			return false;
		}
		httpConfig_.requestTimeout(requestTimeout);

		// read max connections
		uint32_t maxConnections;
		success = config->getConfigParameter("OpcUaWebServerModel.HttpServer.MaxConnections", maxConnections);
		if (!success) {
			maxConnections = 0;
		}
		httpConfig_.maxConnections(maxConnections);

		//
		// now we read the optional ssl attributes
		//

		// read ssl element
		std::string sslString;
		success = config->getConfigParameter("OpcUaWebServerModel.HttpServer.SSL", sslString);
		if (success && boost::to_upper_copy(sslString) == "ON") {
			httpConfig_.ssl(true);
		}

		// read certificate file
		if (httpConfig_.ssl()) {
			std::string csrFile;
			success = config->getConfigParameter("OpcUaWebServerModel.HttpServer.CSRFile", csrFile);
			if (!success) {
				Log(Error, "missing web server parameter in configuration file")
					.parameter("Parameter", "OpcUaWebServerModel.HttpServer.CSRFile")
					.parameter("ConfigurationFile", config->configFileName());
				return false;
			}
			httpConfig_.csrFile(csrFile);
		}

		// read private key
		if (httpConfig_.ssl()) {
			std::string keyFile;
			success = config->getConfigParameter("OpcUaWebServerModel.HttpServer.KeyFile", keyFile);
			if (!success) {
				Log(Error, "missing web server parameter in configuration file")
					.parameter("Parameter", "OpcUaWebServerModel.HttpServer.KeyFile")
					.parameter("ConfigurationFile", config->configFileName());
				return false;
			}
			httpConfig_.keyFile(keyFile);
		}

		return true;
	}

	bool
	WebServer::getIPLoggerConfig(Config* config)
	{
		bool success;

		// read log file name
		std::string logFileName;
		success = config->getConfigParameter("OpcUaWebServerModel.HttpServer.IPLogger.LogFileName", logFileName);
		if (!success) {
			Log(Error, "missing web server parameter in configuration file")
				.parameter("Parameter", "OpcUaWebServerModel.HttpServer.IPLogger.LogFileName")
				.parameter("ConfigurationFile", config->configFileName());
			return false;
		}
		httpConfig_.logFileName(logFileName);

		// read max log file number
		uint32_t maxLogFileNumber;
		success = config->getConfigParameter("OpcUaWebServerModel.HttpServer.IPLogger.MaxLogFileNumber", maxLogFileNumber);
		if (!success) {
			Log(Error, "missing web server parameter in configuration file")
				.parameter("Parameter", "OpcUaServerModel.OpcUaWebServer.HttpServer.IPLogger.MaxLogFileNumber")
				.parameter("ConfigurationFile", config->configFileName());
			return false;
		}
		httpConfig_.maxLogFileNumber(maxLogFileNumber);

		// read max log file size
		uint32_t maxLogFileSize;
		success = config->getConfigParameter("OpcUaWebServerModel.HttpServer.IPLogger.MaxLogFileSize", maxLogFileSize);
		if (!success) {
			Log(Error, "missing web server parameter in configuration file")
				.parameter("Parameter", "OpcUaServerModel.OpcUaWebServer.HttpServer.IPLogger.MaxLogFileSize")
				.parameter("ConfigurationFile", config->configFileName());
			return false;
		}
		httpConfig_.maxLogFileSize(maxLogFileSize);

		// read max log file size
		uint32_t maxIPAge;
		success = config->getConfigParameter("OpcUaWebServerModel.HttpServer.IPLogger.MaxIPAge", maxIPAge);
		if (!success) {
			Log(Error, "missing web server parameter in configuration file")
				.parameter("Parameter", "OpcUaWebServerModel.HttpServer.IPLogger.MaxIPAge")
				.parameter("ConfigurationFile", config->configFileName());
			return false;
		}
		httpConfig_.maxIPAge(maxIPAge);

		return true;
	}

}
