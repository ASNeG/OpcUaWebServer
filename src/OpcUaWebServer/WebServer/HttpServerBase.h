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

#include "OpcUaStackCore/Network/TCPAcceptor.h"
#include "OpcUaStackCore/Network/TCPConnection.h"
#include "OpcUaWebServer/WebServer/HttpConfig.h"
#include "OpcUaWebServer/WebServer/HttpChannel.h"
#include "OpcUaWebServer/WebServer/HttpServerIf.h"

namespace OpcUaWebServer
{

	class HttpServerBase
	{
	  public:
		HttpServerBase(HttpConfig* httpConfig);
		virtual ~HttpServerBase(void);

		bool addHttpServerIf(const std::string& method, HttpServerIf* httpServerIf);

	  protected:
		void receiveRequest(HttpChannel* httpChannel);

	  protected:
		void handleReceiveRequestHeaderTimeout(
			HttpChannel* httpChannel
		);
		void handleReceiveRequestHeader(
			const boost::system::error_code& error,
			std::size_t bytes_transfered,
			HttpChannel* httpChannel
		);
		void handleReceiveRequestContentTimeout(
			HttpChannel* httpChannel
		);
		void handleReceiveRequestContent(
			const boost::system::error_code& error,
			std::size_t bytes_transfered,
			HttpChannel* httpChannel
		);
		void processRequest(
			HttpChannel* httpChannel,
			HttpRequest& httpRequest
		);
		void handleWriteComplete(
			const boost::system::error_code& error,
			std::size_t bytes_transfered,
			HttpChannel* httpChannel
		);
		void closeHttpChannel(
			HttpChannel* httpChannel
		);
		void timeoutHttpChannel(
			HttpChannel* httpChannel,
			const std::string& location
		);
		void performHandshake(
			HttpChannel* httpChannel
		);
		void performHandshakeComplete(
			const boost::system::error_code& error,
			HttpChannel* httpChannel
		);

		virtual void addHttpChannel(uint32_t count) = 0;
		virtual void delHttpChannel(uint32_t count) = 0;

	  protected:
		void initHttpChannel(HttpChannel* httpChannel);
		void cleanupHttpChannel(HttpChannel* httpChannel);

		HttpConfig* httpConfig_;
		OpcUaStackCore::TCPAcceptor tcpAcceptor_;

		typedef std::map<std::string, HttpServerIf*> HttpServerIfMap;
		HttpServerIfMap httpServerIfMap_;

		HttpChannel::Map httpChannelMap_;
	};

}
