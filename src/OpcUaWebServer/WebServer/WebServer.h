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

#ifndef __OpcUaWebServer_WebServer_h__
#define __OpcUaWebServer_WebServer_h__

#include "OpcUaStackCore/Base/IOService.h"
#include "OpcUaStackCore/Base/Config.h"
#include "OpcUaStackCore/Utility/IOThread.h"
#include "OpcUaStackServer/Application/ApplicationInfo.h"
#include "OpcUaWebServer/WebServer/HttpServer.h"
#include "OpcUaWebServer/WebServer/HttpConfig.h"
#include "OpcUaWebServer/WebServer/HttpContent.h"

namespace OpcUaWebServer
{

	class WebServer
	{
	  public:
		typedef std::function<void (bool error)> StartupCompleteCallback;
		typedef std::function<void (bool error)> ShutdownCompleteCallback;

		WebServer(void);
		~WebServer(void);

		void startup(
			OpcUaStackCore::Config* config,
			const OpcUaStackCore::IOThread::SPtr& ioThread,
			const StartupCompleteCallback& startupCompleteCallback
		);
		void shutdown(
			const ShutdownCompleteCallback& shutdownCompleteCallback
		);

	  private:
		void shutdownComplete(bool error);
		bool getHttpConfig(OpcUaStackCore::Config* config);
		bool getIPLoggerConfig(OpcUaStackCore::Config* config);

		ShutdownCompleteCallback shutdownCompleteCallback_;
		HttpServer::SPtr httpServer_;
		HttpContent::SPtr httpContent_;
		HttpConfig httpConfig_;
	};

}

#endif
