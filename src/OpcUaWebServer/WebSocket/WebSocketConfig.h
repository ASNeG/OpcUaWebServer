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

#ifndef __OpcUaWebServer_WebSocketConfig_h__
#define __OpcUaWebServer_WebSocketConfig_h__

#include <stdint.h>
#include <OpcUaStackCore/Utility/IOThread.h>

namespace OpcUaWebServer
{

	class WebSocketConfig
	{
	  public:
		WebSocketConfig(void);
		virtual ~WebSocketConfig(void);

		void enable(bool enable);
		bool enable(void);
		void address(const std::string& address);
		std::string& address(void);
		void port(uint32_t port);
		uint32_t port(void);
		void requestTimeout(uint32_t requestTimeout);
		uint32_t requestTimeout(void);
		void idleTimeout(uint32_t requestTimeout);
		uint32_t idleTimeout(void);
		void ioThread(const OpcUaStackCore::IOThread::SPtr& ioThread);
		OpcUaStackCore::IOThread::SPtr& ioThread(void);
		void strand(const boost::shared_ptr<boost::asio::io_service::strand>& strand);
		boost::shared_ptr<boost::asio::io_service::strand>& strand(void);
		void maxConnections(uint32_t maxConnections);
		uint32_t maxConnections(void);
		void maxPacketLength(uint32_t maxPacketLength);
		uint32_t maxPacketLength(void);

		void ssl(bool ssl);
		bool ssl(void);
		void csrFile(const std::string& csrFile);
		std::string csrFile(void);
		void keyFile(const std::string& keyFile);
		std::string keyFile(void);

	  private:
		bool enable_;
		std::string address_;
		uint32_t port_;
		uint32_t requestTimeout_;
		uint32_t idleTimeout_;
		OpcUaStackCore::IOThread::SPtr ioThread_;
		boost::shared_ptr<boost::asio::io_service::strand> strand_;
		uint32_t maxConnections_ = 0;
		uint32_t maxPacketLength_ = 0;

		// security settings
		bool ssl_ = false;				// activate or deactivate SSL security
		std::string csrFile_ = "";		// server SSL certificate
		std::string keyFile_ = "";		// server private key
	};

}

#endif
