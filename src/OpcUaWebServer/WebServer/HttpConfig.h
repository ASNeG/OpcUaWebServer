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

#ifndef __OpcUaWebServer_HttpConfig_h__
#define __OpcUaWebServer_HttpConfig_h__

#include <stdint.h>
#include "OpcUaStackCore/Utility/IOThread.h"

using namespace OpcUaStackCore;

namespace OpcUaWebServer
{

	class HttpConfig
	{
	  public:
		HttpConfig(void);
		virtual ~HttpConfig(void);

		void address(const std::string& address);
		std::string& address(void);
		void port(uint32_t port);
		uint32_t port(void);
		void ioThread(const IOThread::SPtr& ioThread);
		IOThread::SPtr& ioThread(void);
		void webDirectory(const std::string& webDirectory);
		std::string webDirectory(void);
		void requestTimeout(uint32_t requestTimeout);
		uint32_t requestTimeout(void);

		void logFileName(const std::string& logFileName);
		std::string logFileName(void);
		void maxLogFileNumber(uint32_t maxLogFileNumber);
		uint32_t maxLogFileNumber(void);
		void maxLogFileSize(uint32_t maxLogFileSize);
		uint32_t maxLogFileSize(void);
		void maxIPAge(uint32_t maxIPAge);
		uint32_t maxIPAge(void);

	  private:
		// http channel configuration
		std::string address_;
		uint32_t port_;
		IOThread::SPtr ioThread_;
		std::string webDirectory_;
		uint32_t requestTimeout_;

		// IP logger configuration
		std::string logFileName_;
		uint32_t maxLogFileNumber_;
		uint32_t maxLogFileSize_;
		uint32_t maxIPAge_;
	};

}

#endif
