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
#include "OpcUaWebServer/WebSocket/WebSocketConfig.h"
#include "OpcUaWebServer/WebSocket/WebSocketChannel.h"
#include "OpcUaWebServer/WebSocket/WebSocketMessage.h"

namespace OpcUaWebServer
{

	class DLLEXPORT WebSocketServerBase
	{
	  public:
		typedef enum {
			OP_CONTINUATION_FRAME = 0,
			OP_TEXT_FRAME = 1,
			OP_BINARY_FRAME = 2,
			OP_CLOSE_FRAME = 8,
			OP_PING_FRAME = 9
		} OpCode;

		typedef std::function<void (bool error)> SendCompleteCallback;
		typedef std::function<void (WebSocketMessage::SPtr& webSocketMessag)> ReceiveMessageCallback;

		WebSocketServerBase(WebSocketConfig* webSocketConfig);
		virtual ~WebSocketServerBase(void);

		void disconnect(uint32_t channelId);
		void receiveMessageCallback(
			const ReceiveMessageCallback& receiveMessageCallback
		);
		void sendMessage(
			WebSocketMessage::SPtr& webSocketMessage,
			const SendCompleteCallback& sendCompleteCallback
		);

		virtual void addWebSocketChannel(uint32_t count) = 0;
		virtual void delWebSocketChannel(uint32_t count) = 0;

	  protected:
		void receiveHandshake(WebSocketChannel* webSocketChannel);

	  protected:
		void initWebSocketChannel(WebSocketChannel* webSocketChannel);
		void cleanupWebSocketChannel(WebSocketChannel* webSocketChannel);
		void closeWebSocketChannel(WebSocketChannel* httpChannel);

		//
		// handle handshake
		//
		void handleReceiveHandshakeHeader(
			const boost::system::error_code& error,
			std::size_t bytes_transfered,
			WebSocketChannel* webSocketChannel
		);
		void handleReceiveHandshakeContent(
			const boost::system::error_code& error,
			std::size_t bytes_transfered,
			WebSocketChannel* webSocketChannel
		);
		void performHandshake(
			WebSocketChannel* webSocketChannel
		);
		void performHandshakeComplete(
			const boost::system::error_code& error,
			WebSocketChannel* webSocketChannel
		);
		void processHandshake(
			WebSocketChannel* webSocketChannel,
			WebSocketRequest& webSocketRequest
		);
		void handleWriteComplete(
			const boost::system::error_code& error,
			std::size_t bytes_transfered,
			WebSocketChannel* webSocketChannel
		);
		int base64Encode(
			const unsigned char* buffer,
			size_t length,
			char** b64text
		);

		void requestTimeoutWebSocketChannel(WebSocketChannel* webSocketChannel, const std::string& location);


		//
		// handle receive message
		//
		void receiveMessage(WebSocketChannel* webSocketChannel);
		void handleReceiveMessageHeader(const boost::system::error_code& error, std::size_t bytes_transfered, WebSocketChannel* webSocketChannel);
		void handleReceiveMessageLength2(const boost::system::error_code& error, std::size_t bytes_transfered, WebSocketChannel* webSocketChannel);
		void handleReceiveMessageLength8(const boost::system::error_code& error, std::size_t bytes_transfered, WebSocketChannel* webSocketChannel);
		void handleReceiveMessageContent(const boost::system::error_code& error, std::size_t bytes_transfered, WebSocketChannel* webSocketChannel);

		void idleTimeoutWebSocketChannel(WebSocketChannel* webSocketChannel, const std::string& location);


		//
		// handle send message
		//
		bool sendMessage(
			WebSocketMessage::SPtr& webSocketMessage,
			WebSocketChannel* webSocketChannel,
			const SendCompleteCallback& sendCompleteCallback,
			char headerByte = 0x00
		);
		//
		// handle send message
		//
		bool sendMessageQueue(
			WebSocketMessage::SPtr& webSocketMessage,
			WebSocketChannel* webSocketChannel,
			const SendCompleteCallback& sendCompleteCallback,
			char headerByte = 0x00
		);

		void handleWriteMessageComplete(
			const boost::system::error_code& error,
			std::size_t bytes_transferred,
			WebSocketChannel* webSocketChannel,
			const SendCompleteCallback& sendCompleteCallback
		);

		WebSocketConfig* webSocketConfig_;
		OpcUaStackCore::TCPAcceptor tcpAcceptor_;

		ReceiveMessageCallback receiveMessageCallback_;

		WebSocketChannel::Map webSocketChannelMap_;
	};

}
