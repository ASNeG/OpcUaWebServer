/*
   Copyright 2015-2017 Kai Huebl (kai@huebl-sgh.de)

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
	{
	}

	Library::~Library(void)
	{
	}

	bool
	Library::startup(void)
	{
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

		return true;
	}

	bool
	Library::shutdown(void)
	{
		Log(Debug, "Library::shutdown");

		if (!ioThread_->shutdown()) return false;
		ioThread_.reset();

		if (!webServer_.shutdown()) return false;

		return true;
	}

	std::string
	Library::version(void)
	{
		std::stringstream version;

		version << LIBRARY_VERSION_MAJOR << "." << LIBRARY_VERSION_MINOR << "." << LIBRARY_VERSION_PATCH;
		return version.str();
	}

}

extern "C" DLLEXPORT void  init(ApplicationIf** applicationIf) {
    *applicationIf = new OpcUaWebServer::Library();
}

