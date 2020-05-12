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

#include <openssl/sha.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>
#include <boost/make_shared.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

#include "OpcUaStackCore/Base/Log.h"
#include "OpcUaWebServer/WebSocket/WebSocketServerBase.h"

using namespace OpcUaStackCore;

namespace OpcUaWebServer
{

	WebSocketServerBase::WebSocketServerBase(WebSocketConfig* webSocketConfig)
	: webSocketConfig_(webSocketConfig)
	, tcpAcceptor_(webSocketConfig->ioThread()->ioService()->io_service(), webSocketConfig->address(), webSocketConfig->port())
	, receiveMessageCallback_()
	, webSocketChannelMap_()
	{
	}

	WebSocketServerBase::~WebSocketServerBase(void)
	{
	}

	void
	WebSocketServerBase::disconnect(uint32_t channelId)
	{
		Log(Error, "web socket server disconnect")
		    .parameter("ChannelId", channelId);

		WebSocketChannel* webSocketChannel;

		{
			// get web socket channel
			auto it = webSocketChannelMap_.find(channelId);
			if (it == webSocketChannelMap_.end()) {
				Log(Error, "web socket channel not exist - ignore disconnect")
					.parameter("ChannelId", channelId);
				return;
			}
			webSocketChannel = it->second;

			// stop timer
			webSocketConfig_->ioThread()->slotTimer()->stop(webSocketChannel->slotTimerElement_);

			// close channel
			webSocketChannel->socket().close();
		}
	}

	void
	WebSocketServerBase::receiveMessageCallback(
		const ReceiveMessageCallback& receiveMessageCallback
	)
	{
		receiveMessageCallback_ = receiveMessageCallback;
	}

	void
	WebSocketServerBase::sendMessage(
		WebSocketMessage::SPtr& webSocketMessage,
		const SendCompleteCallback& sendCompleteCallback
	)
	{
		WebSocketChannel* webSocketChannel;

		{
			// get web socket channel
			auto it = webSocketChannelMap_.find(webSocketMessage->channelId_);
			if (it == webSocketChannelMap_.end()) {
				Log(Error, "web socket channel not exist - ignore send message")
					.parameter("ChannelId", webSocketMessage->channelId_);
				sendCompleteCallback(false);
			}
			webSocketChannel = it->second;
		}

		// check shutdown flag
		if (webSocketChannel->shutdown_) {
			return;
		}

		// send message to client
		sendMessage(webSocketMessage, webSocketChannel, sendCompleteCallback);
	}

	void
	WebSocketServerBase::closeWebSocketChannel(WebSocketChannel* webSocketChannel)
	{
		Log(Info, "close web socket channel")
			.parameter("ChannelId", webSocketChannel->id_)
			.parameter("QueueSize", webSocketChannel->sendQueue_.size())
			.parameter("asyncWrite", webSocketChannel->asyncWrite_)
			.parameter("asyncRead", webSocketChannel->asyncRead_);

		if (webSocketChannel->asyncWrite_ || webSocketChannel->asyncRead_) {
			webSocketChannel->shutdown_ = true;
			return;
		}

		// stop timer
		webSocketConfig_->ioThread()->slotTimer()->stop(webSocketChannel->slotTimerElement_);

		// close channel
		webSocketChannel->socket().close();

		// remove web socket from channel map
		webSocketChannel->sendQueue_.clear();
		cleanupWebSocketChannel(webSocketChannel);

		// send channel close message to opc ua client
		auto webSocketMessage = boost::make_shared<WebSocketMessage>();
		webSocketMessage->channelId_ = webSocketChannel->id_;
		webSocketMessage->message_ = "{\"Header\":{\"MessageType\": \"CHANNELCLOSE_MESSAGE\",\"ClientHandle\": \"---\"},\"Body\":{}}";
		if (receiveMessageCallback_) {
			receiveMessageCallback_(webSocketMessage);
		}

		// delete channel
		delete webSocketChannel;
	}

	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	//
	// web socket managament
	//
	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	void
	WebSocketServerBase::initWebSocketChannel(WebSocketChannel* webSocketChannel)
	{
		webSocketChannelMap_.insert(std::make_pair(webSocketChannel->id_, webSocketChannel));
		Log(Debug, "create web socket")
		    .parameter("ChannelId", webSocketChannel->id_)
			.parameter("NumberConnections", webSocketChannelMap_.size());

		addWebSocketChannel(webSocketChannelMap_.size());
	}

	void
	WebSocketServerBase::cleanupWebSocketChannel(WebSocketChannel* webSocketChannel)
	{
		Log(Debug, "delete web socket")
		    .parameter("ChannelId", webSocketChannel->id_)
			.parameter("NumberConnections", webSocketChannelMap_.size());

		auto it = webSocketChannelMap_.find(webSocketChannel->id_);
		if (it != webSocketChannelMap_.end()) {
			webSocketChannelMap_.erase(it);
			delWebSocketChannel(webSocketChannelMap_.size());
		}
	}

	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	//
	// handle handshake
	//
	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	void
	WebSocketServerBase::performHandshake(WebSocketChannel* webSocketChannel)
	{
		// start timer
		webSocketChannel->slotTimerElement_->expireFromNow(webSocketConfig_->requestTimeout());
		webSocketChannel->slotTimerElement_->timeoutCallback(
			webSocketConfig_->strand(),
			[this, webSocketChannel](void) { requestTimeoutWebSocketChannel(webSocketChannel, "perform handshake"); }
		);
		webSocketConfig_->ioThread()->slotTimer()->start(webSocketChannel->slotTimerElement_);

		webSocketChannel->socket().performHandshake(
			webSocketConfig_->strand(),
			[this, webSocketChannel](const boost::system::error_code& error) {
			    performHandshakeComplete(error, webSocketChannel);
		    }
		);
	}

	void
	WebSocketServerBase::performHandshakeComplete(
		const boost::system::error_code& error,
		WebSocketChannel* webSocketChannel
	)
	{
		if (webSocketChannel->timeout_ || error || webSocketChannel->shutdown_) {

			if (error) {
				Log(Debug, "WebSocketServer receive request header error; close channel")
					.parameter("Address", webSocketChannel->partner_.address().to_string())
					.parameter("Port", webSocketChannel->partner_.port())
					.parameter("ChannelId", webSocketChannel->id_);
			}

			closeWebSocketChannel(webSocketChannel);
			return;
		}

		// stop request timer
		webSocketConfig_->ioThread()->slotTimer()->stop(webSocketChannel->slotTimerElement_);

		receiveHandshake(webSocketChannel);
	}

	void
	WebSocketServerBase::receiveHandshake(WebSocketChannel* webSocketChannel)
	{
		// start timer
		webSocketChannel->slotTimerElement_->expireFromNow(webSocketConfig_->requestTimeout());
		webSocketChannel->slotTimerElement_->timeoutCallback(
			webSocketConfig_->strand(),
			[this, webSocketChannel](void) { requestTimeoutWebSocketChannel(webSocketChannel, "request header"); }
		);
		webSocketConfig_->ioThread()->slotTimer()->start(webSocketChannel->slotTimerElement_);

		webSocketChannel->socket().async_read_until(
			webSocketConfig_->strand(),
			webSocketChannel->recvBuffer_,
			"\r\n\r\n",
			[this, webSocketChannel](const boost::system::error_code& error, std::size_t bytes_transfered) {
				handleReceiveHandshakeHeader(error, bytes_transfered, webSocketChannel);
			}
		);
	}

	void
	WebSocketServerBase::handleReceiveHandshakeHeader(
		const boost::system::error_code& error,
		std::size_t bytes_transfered,
		WebSocketChannel* webSocketChannel
	)
	{
		if (webSocketChannel->timeout_ || error || webSocketChannel->shutdown_) {

			if (error) {
				Log(Debug, "WebSocketServer receive request header error; close channel")
					.parameter("Address", webSocketChannel->partner_.address().to_string())
					.parameter("Port", webSocketChannel->partner_.port())
					.parameter("ChannelId", webSocketChannel->id_)
					.parameter("Size", bytes_transfered);
			}

			closeWebSocketChannel(webSocketChannel);
			return;
		}

		if (webSocketConfig_->maxPacketLength() != 0 && bytes_transfered > webSocketConfig_->maxPacketLength()) {
			Log(Debug, "WebSocketServer receive invalid packet size")
				.parameter("Address", webSocketChannel->partner_.address().to_string())
				.parameter("Port", webSocketChannel->partner_.port())
				.parameter("ChannelId", webSocketChannel->id_)
				.parameter("MaxPacketSize", webSocketConfig_->maxPacketLength())
				.parameter("PacketSize", bytes_transfered);

			closeWebSocketChannel(webSocketChannel);
			return;
		}

		// stop request timer
		webSocketConfig_->ioThread()->slotTimer()->stop(webSocketChannel->slotTimerElement_);

		size_t numAdditionalBytes = webSocketChannel->recvBuffer_.size() - bytes_transfered;
		std::istream is(&webSocketChannel->recvBuffer_);

		//
		// read request header
		//
		if (!webSocketChannel->webSocketRequest_.decodeRequestHeader(is)) {
			Log(Debug, "WebSocketServer decode request error; close channel")
				.parameter("Address", webSocketChannel->partner_.address().to_string())
				.parameter("Port", webSocketChannel->partner_.port());

			closeWebSocketChannel(webSocketChannel);
		}

		//
		// read request content
		//
		std::string contentLengthString;
		if (!webSocketChannel->webSocketRequest_.getHeaderPara("Content-Length", contentLengthString)) {
			processHandshake(webSocketChannel, webSocketChannel->webSocketRequest_);
			return;
		}

		bool success = true;
		size_t contentLength;
		try {
			contentLength = boost::lexical_cast<size_t>(contentLengthString);
		} catch(boost::bad_lexical_cast&) {
			success = false;
			return;
		}

		if (!success) {
			Log(Debug, "WebSocketServer content length error in request; close channel")
				.parameter("Address", webSocketChannel->partner_.address().to_string())
				.parameter("Port", webSocketChannel->partner_.port());

			closeWebSocketChannel(webSocketChannel);
		}

		if (webSocketConfig_->maxPacketLength() != 0 && (contentLength-numAdditionalBytes) > webSocketConfig_->maxPacketLength()) {
			Log(Debug, "WebSocketServer receive invalid content size")
				.parameter("Address", webSocketChannel->partner_.address().to_string())
				.parameter("Port", webSocketChannel->partner_.port())
				.parameter("ChannelId", webSocketChannel->id_)
				.parameter("MaxPacketSize", webSocketConfig_->maxPacketLength())
				.parameter("ContentSize", (contentLength-numAdditionalBytes));

			closeWebSocketChannel(webSocketChannel);
			return;
		}

		// start request timer
		webSocketChannel->slotTimerElement_->expireFromNow(webSocketConfig_->requestTimeout());
		webSocketChannel->slotTimerElement_->timeoutCallback(
			webSocketConfig_->strand(),
			[this, webSocketChannel](void){ requestTimeoutWebSocketChannel(webSocketChannel, "request content"); }
		);
		webSocketConfig_->ioThread()->slotTimer()->start(webSocketChannel->slotTimerElement_);

		webSocketChannel->socket().async_read_exactly(
			webSocketConfig_->strand(),
			webSocketChannel->recvBuffer_,
			contentLength-numAdditionalBytes,
			[this, webSocketChannel](const boost::system::error_code& error, std::size_t bytes_transfered) {
				handleReceiveHandshakeContent(error, bytes_transfered, webSocketChannel);
			}
		);
	}

	void
	WebSocketServerBase::handleReceiveHandshakeContent(
		const boost::system::error_code& error,
		std::size_t bytes_transfered,
		WebSocketChannel* webSocketChannel
	)
	{
		if (webSocketChannel->timeout_ || error || webSocketChannel->shutdown_) {

			if (error) {
				Log(Debug, "WebSocketServer receive request content error; close channel")
					.parameter("Address", webSocketChannel->partner_.address().to_string())
					.parameter("Port", webSocketChannel->partner_.port())
					.parameter("ChannelId", webSocketChannel->id_);
			}

			closeWebSocketChannel(webSocketChannel);
			return;
		}

		// stop request timer
		webSocketConfig_->ioThread()->slotTimer()->stop(webSocketChannel->slotTimerElement_);

		//
		// read content
		//
		processHandshake(webSocketChannel, webSocketChannel->webSocketRequest_);
	}

	void
	WebSocketServerBase::processHandshake(WebSocketChannel* webSocketChannel, WebSocketRequest& webSocketRequest)
	{
		// determine request method
		std::string method = webSocketRequest.method();
		boost::to_upper(method);

		// HTTP/1.1 101 Web Socket Protocol Handshake\r\n
        // Upgrade: websocket\r\n
        // Connection: Upgrade\r\n
        // Sec-WebSocket-Accept: ...

		// get Sec-WebSocket-Key
		std::string webSocketKey;
		if (!webSocketRequest.getHeaderPara("Sec-WebSocket-Key", webSocketKey)) {
			Log(Debug, "WebSocketServer request without parameter Sec-WebSocket-key; close channel")
				.parameter("Address", webSocketChannel->partner_.address().to_string())
				.parameter("Port", webSocketChannel->partner_.port())
				.parameter("ChannelId", webSocketChannel->id_);

			closeWebSocketChannel(webSocketChannel);
			return;
		}

		const std::string ws_magic_string="258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
		webSocketKey = webSocketKey + ws_magic_string;

		unsigned char key[20];
		SHA_CTX context;
		SHA1_Init(&context);
		SHA1_Update(&context, webSocketKey.c_str(), webSocketKey.length());
		SHA1_Final(key, &context);

		char* b64text;
		base64Encode(key, 20, &b64text);

		webSocketChannel->webSocketResponse_.statusCode(101);
		webSocketChannel->webSocketResponse_.statusString("Web Socket Protocol Handshake");
		webSocketChannel->webSocketResponse_.setHeaderPara("Upgrade", "websocket");
		webSocketChannel->webSocketResponse_.setHeaderPara("Connection", "Upgrade");
		webSocketChannel->webSocketResponse_.setHeaderPara("Sec-WebSocket-Accept", b64text);
		webSocketChannel->webSocketResponse_.setHeaderPara("Sec-WebSocket-Protocol", "json");
		delete b64text;

		// send response
		std::ostream os(&webSocketChannel->sendBuffer_);
		webSocketChannel->webSocketResponse_.encodeRequestHeader(os);
		webSocketChannel->socket().async_write(
			webSocketConfig_->strand(),
			webSocketChannel->sendBuffer_,
			[this, webSocketChannel](const boost::system::error_code& error, std::size_t bytes_transfered){
				handleWriteComplete(error, bytes_transfered, webSocketChannel);
		    }
		);

		return;
	}

	void
	WebSocketServerBase::handleWriteComplete(
		const boost::system::error_code& error,
		std::size_t bytes_transfered,
		WebSocketChannel* webSocketChannel
	)
	{
		if (error) {
			Log(Debug, "WebSocketServer send response error; close channel")
				.parameter("Address", webSocketChannel->partner_.address().to_string())
				.parameter("Port", webSocketChannel->partner_.port())
				.parameter("ChannelId", webSocketChannel->id_);

			closeWebSocketChannel(webSocketChannel);
			return;
		}

		receiveMessage(webSocketChannel);
	}

	int
	WebSocketServerBase::base64Encode(const unsigned char* buffer, size_t length, char** b64text)
	{
		BIO *bio, *b64;
		BUF_MEM *bufferPtr;
		b64 = BIO_new(BIO_f_base64());
		bio = BIO_new(BIO_s_mem());
		bio = BIO_push(b64, bio);
		BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
		BIO_write(bio, buffer, length);
		BIO_flush(bio);
		BIO_get_mem_ptr(bio, &bufferPtr);
		BIO_set_close(bio, BIO_NOCLOSE);

		*b64text = (char*) malloc((bufferPtr->length + 1) * sizeof(char));
		memcpy(*b64text, bufferPtr->data, bufferPtr->length);
		(*b64text)[bufferPtr->length] = '\0';

		BIO_free_all(bio);
		return (0); //success
	}

	void
	WebSocketServerBase::requestTimeoutWebSocketChannel(WebSocketChannel* webSocketChannel, const std::string& location)
	{
		Log(Debug, "WebSocketServer request timeout; close channel")
			.parameter("Address", webSocketChannel->partner_.address().to_string())
			.parameter("Port", webSocketChannel->partner_.port())
			.parameter("Location", location)
			.parameter("ChannelId", webSocketChannel->id_);

		webSocketChannel->timeout_ = true;
		webSocketChannel->socket().cancel();
	}

	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	//
	// handle receive messages
	//
	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	void
	WebSocketServerBase::receiveMessage(WebSocketChannel* webSocketChannel)
	{
		// start request timer
		webSocketChannel->slotTimerElement_->expireFromNow(webSocketConfig_->idleTimeout());
		webSocketChannel->slotTimerElement_->timeoutCallback(
			webSocketConfig_->strand(),
			[this, webSocketChannel](void){ idleTimeoutWebSocketChannel(webSocketChannel, "json header"); }
		);
		webSocketConfig_->ioThread()->slotTimer()->start(webSocketChannel->slotTimerElement_);

		webSocketChannel->asyncRead_ = true;
		webSocketChannel->socket().async_read_exactly(
			webSocketConfig_->strand(),
			webSocketChannel->recvBuffer_,
			2,
			[this, webSocketChannel](const boost::system::error_code& error, std::size_t bytes_transfered) {
				handleReceiveMessageHeader(error, bytes_transfered, webSocketChannel);
		    }
		);
	}

	void
	WebSocketServerBase::handleReceiveMessageHeader(
		const boost::system::error_code& error,
		std::size_t bytes_transfered,
		WebSocketChannel* webSocketChannel
	)
	{
		webSocketChannel->asyncRead_ = false;

		if (webSocketChannel->timeout_ || error || webSocketChannel->shutdown_) {
			Log(Debug, "WebSocketServer handle receive message header timeout, error or shutdown");
			closeWebSocketChannel(webSocketChannel);
			return;
		}

		// stop idle timer
		webSocketConfig_->ioThread()->slotTimer()->stop(webSocketChannel->slotTimerElement_);

		if (error) {
			Log(Debug, "WebSocketServer receive message header error; close channel")
				.parameter("Address", webSocketChannel->partner_.address().to_string())
				.parameter("Port", webSocketChannel->partner_.port())
				.parameter("ChannelId", webSocketChannel->id_);

			closeWebSocketChannel(webSocketChannel);
			return;
		}

		std::istream is(&webSocketChannel->recvBuffer_);
		char headerBytes[2];
		is.read(headerBytes, 2);

		bool fin = (headerBytes[0] & 0x80) == 0x80;	// true - last frame
		webSocketChannel->opcode_ = headerBytes[0] & 0x0F;		// 1 - text frame
		bool mask = (headerBytes[1] & 0x80) == 0x80;	// true - mask
		uint32_t length = headerBytes[1] & 0x7f;

		if (webSocketChannel->opcode_ == OP_CONTINUATION_FRAME) {
			Log(Debug, "WebSocketServer do not support continuation frame messages; close channel")
				.parameter("Address", webSocketChannel->partner_.address().to_string())
				.parameter("Port", webSocketChannel->partner_.port())
				.parameter("ChannelId", webSocketChannel->id_);

			closeWebSocketChannel(webSocketChannel);
			return;
		}

		if (webSocketChannel->opcode_ == OP_BINARY_FRAME) {
			Log(Error, "WebSocketServer do not support binary frame messages; close channel")
				.parameter("Address", webSocketChannel->partner_.address().to_string())
				.parameter("Port", webSocketChannel->partner_.port())
				.parameter("ChannelId", webSocketChannel->id_);

			closeWebSocketChannel(webSocketChannel);
			return;
		}

		if (webSocketChannel->opcode_ == OP_CLOSE_FRAME) {
			Log(Debug, "WebSocketServer receive close frame messages; close channel")
				.parameter("Address", webSocketChannel->partner_.address().to_string())
				.parameter("Port", webSocketChannel->partner_.port())
				.parameter("ChannelId", webSocketChannel->id_);

			closeWebSocketChannel(webSocketChannel);
			return;
		}

		if (webSocketChannel->opcode_ != OP_TEXT_FRAME && webSocketChannel->opcode_ != OP_PING_FRAME) {
			Log(Debug, "WebSocketServer do not support continuation text messages; close channel")
				.parameter("Address", webSocketChannel->partner_.address().to_string())
				.parameter("Port", webSocketChannel->partner_.port())
				.parameter("ChannelId", webSocketChannel->id_);

			closeWebSocketChannel(webSocketChannel);
			return;
		}

		if (length <= 125) {
			// start request timer
			webSocketChannel->slotTimerElement_->expireFromNow(webSocketConfig_->requestTimeout());
			webSocketChannel->slotTimerElement_->timeoutCallback(
				webSocketConfig_->strand(),
				[this, webSocketChannel](){ idleTimeoutWebSocketChannel(webSocketChannel, "json content"); }
			);
			webSocketConfig_->ioThread()->slotTimer()->start(webSocketChannel->slotTimerElement_);

			webSocketChannel->asyncRead_ = true;
			webSocketChannel->socket().async_read_exactly(
				webSocketConfig_->strand(),
				webSocketChannel->recvBuffer_,
				length+4,
				[this, webSocketChannel](const boost::system::error_code& error, std::size_t bytes_transfered) {
					handleReceiveMessageContent(error, bytes_transfered, webSocketChannel);
			    }
			);
			return;
		}

		if (length == 126) {
			// start request timer
			webSocketChannel->slotTimerElement_->expireFromNow(webSocketConfig_->requestTimeout());
			webSocketChannel->slotTimerElement_->timeoutCallback(
				webSocketConfig_->strand(),
				[this, webSocketChannel](){ idleTimeoutWebSocketChannel(webSocketChannel, "json message length2"); }
			);
			webSocketConfig_->ioThread()->slotTimer()->start(webSocketChannel->slotTimerElement_);

			webSocketChannel->asyncRead_ = true;
			webSocketChannel->socket().async_read_exactly(
				webSocketConfig_->strand(),
				webSocketChannel->recvBuffer_,
				2,
				[this, webSocketChannel](const boost::system::error_code& error, std::size_t bytes_transfered) {
					handleReceiveMessageLength2(error, bytes_transfered, webSocketChannel);
				}
			);
			return;
		}

		if (length == 127) {
			// start request timer
			webSocketChannel->slotTimerElement_->expireFromNow(webSocketConfig_->requestTimeout());
			webSocketChannel->slotTimerElement_->timeoutCallback(
				webSocketConfig_->strand(),
				[this, webSocketChannel](){ idleTimeoutWebSocketChannel(webSocketChannel, "json message length2"); }
			);
			webSocketConfig_->ioThread()->slotTimer()->start(webSocketChannel->slotTimerElement_);

			webSocketChannel->asyncRead_ = true;
			webSocketChannel->socket().async_read_exactly(
				webSocketConfig_->strand(),
				webSocketChannel->recvBuffer_,
				8,
				[this, webSocketChannel](const boost::system::error_code& error, std::size_t bytes_transfered) {
					handleReceiveMessageLength8(error, bytes_transfered, webSocketChannel);
				}
			);
			return;
		}

	}

	void
	WebSocketServerBase::handleReceiveMessageLength2(
		const boost::system::error_code& error,
		std::size_t bytes_transfered,
		WebSocketChannel* webSocketChannel
	)
	{
		webSocketChannel->asyncRead_ = false;

		if (webSocketChannel->timeout_ || error || webSocketChannel->shutdown_) {
			Log(Debug, "WebSocketServer handle receive message header length2 timeout, error or shutdown");
			closeWebSocketChannel(webSocketChannel);
			return;
		}

		// stop request timer
		webSocketConfig_->ioThread()->slotTimer()->stop(webSocketChannel->slotTimerElement_);

		if (error) {
			Log(Debug, "WebSocketServer receive message content2 error; close channel")
				.parameter("Address", webSocketChannel->partner_.address().to_string())
				.parameter("Port", webSocketChannel->partner_.port())
				.parameter("ChannelId", webSocketChannel->id_);

			closeWebSocketChannel(webSocketChannel);
			return;
		}

		// read two byte length
		std::istream is(&webSocketChannel->recvBuffer_);
		char lengthBytes[2];
		is.read(lengthBytes, 2);

		uint32_t length = (lengthBytes[0] & 0xFF) * 256
				        + (lengthBytes[1] & 0xFF);

		// start request timer
		webSocketChannel->slotTimerElement_->expireFromNow(webSocketConfig_->requestTimeout());
		webSocketChannel->slotTimerElement_->timeoutCallback(
			webSocketConfig_->strand(),
			[this, webSocketChannel](){ idleTimeoutWebSocketChannel(webSocketChannel, "json content"); }
		);
		webSocketConfig_->ioThread()->slotTimer()->start(webSocketChannel->slotTimerElement_);

		webSocketChannel->asyncRead_ = true;
		webSocketChannel->socket().async_read_exactly(
			webSocketConfig_->strand(),
			webSocketChannel->recvBuffer_,
			length+4,
			[this, webSocketChannel](const boost::system::error_code& error, std::size_t bytes_transfered) {
				handleReceiveMessageContent(error, bytes_transfered, webSocketChannel);
			}
		);
	}

	void
	WebSocketServerBase::handleReceiveMessageLength8(const boost::system::error_code& error, std::size_t bytes_transfered, WebSocketChannel* webSocketChannel)
	{
		webSocketChannel->asyncRead_ = false;

		if (webSocketChannel->timeout_ || error || webSocketChannel->shutdown_) {
			Log(Debug, "WebSocketServer handle receive message header length8 timeout, error or shutdown");
			closeWebSocketChannel(webSocketChannel);
			return;
		}

		// stop request timer
		webSocketConfig_->ioThread()->slotTimer()->stop(webSocketChannel->slotTimerElement_);

		if (error) {
			Log(Debug, "WebSocketServer receive message content2 error; close channel")
				.parameter("Address", webSocketChannel->partner_.address().to_string())
				.parameter("Port", webSocketChannel->partner_.port())
				.parameter("ChannelId", webSocketChannel->id_);

			closeWebSocketChannel(webSocketChannel);
			return;
		}

		// read two byte length
		std::istream is(&webSocketChannel->recvBuffer_);
		char lengthBytes[8];
		is.read(lengthBytes, 8);

		uint64_t length = (static_cast<uint64_t>((lengthBytes[0] & 0xFF)) << 56)
						+ (static_cast<uint64_t>((lengthBytes[1] & 0xFF)) << 48)
						+ (static_cast<uint64_t>((lengthBytes[2] & 0xFF)) << 40)
						+ (static_cast<uint64_t>((lengthBytes[3] & 0xFF)) << 32)
						+ (static_cast<uint64_t>((lengthBytes[4] & 0xFF)) << 24)
						+ (static_cast<uint64_t>((lengthBytes[5] & 0xFF)) << 16)
				        + (static_cast<uint64_t>((lengthBytes[6] & 0xFF)) << 8)
				        + (lengthBytes[7] & 0xFF);

		// start request timer
		webSocketChannel->slotTimerElement_->expireFromNow(webSocketConfig_->requestTimeout());
		webSocketChannel->slotTimerElement_->timeoutCallback(
			webSocketConfig_->strand(),
			[this, webSocketChannel](){ idleTimeoutWebSocketChannel(webSocketChannel, "json content"); }
		);
		webSocketConfig_->ioThread()->slotTimer()->start(webSocketChannel->slotTimerElement_);

		webSocketChannel->asyncRead_ = true;
		webSocketChannel->socket().async_read_exactly(
			webSocketConfig_->strand(),
			webSocketChannel->recvBuffer_,
			length+4,
			[this, webSocketChannel](const boost::system::error_code& error, std::size_t bytes_transfered) {
				handleReceiveMessageContent(error, bytes_transfered, webSocketChannel);
			}
		);
	}

	void
	WebSocketServerBase::handleReceiveMessageContent(const boost::system::error_code& error, std::size_t bytes_transfered, WebSocketChannel* webSocketChannel)
	{
		webSocketChannel->asyncRead_ = false;

		if (webSocketChannel->timeout_ || error || webSocketChannel->shutdown_) {
			Log(Debug, "WebSocketServer handle receive message content timeout, error or shutdown");
			closeWebSocketChannel(webSocketChannel);
			return;
		}

		// stop request timer
		webSocketConfig_->ioThread()->slotTimer()->stop(webSocketChannel->slotTimerElement_);

		if (error) {
			Log(Debug, "WebSocketServer receive message content error; close channel")
				.parameter("Address", webSocketChannel->partner_.address().to_string())
				.parameter("Port", webSocketChannel->partner_.port())
				.parameter("ChannelId", webSocketChannel->id_);

			closeWebSocketChannel(webSocketChannel);
			return;
		}

		auto webSocketMessage = boost::make_shared<WebSocketMessage>();
		webSocketMessage->channelId_ = webSocketChannel->id_;
		webSocketMessage->message_ = "";

		std::istream is(&webSocketChannel->recvBuffer_);

		char maskingKey[4];
		is.read(maskingKey, 4);
		bytes_transfered -= 4;

		uint32_t pos = 0;
		while (bytes_transfered > 0) {
			char buffer[10000];
			uint32_t bufferLen = 10000;

			if (bytes_transfered < bufferLen) bufferLen = bytes_transfered;
			bytes_transfered -= bufferLen;

			is.read(buffer, bufferLen);

			// demasking
			for (uint32_t idx=0; idx<bufferLen; idx++) {
				buffer[idx] = buffer[idx] ^ maskingKey[pos%4];
				pos++;
			}

			webSocketMessage->message_.append(buffer, bufferLen);
		}

		if (webSocketChannel->opcode_ == OP_PING_FRAME) {
			// send answer to client
			sendMessage(
				webSocketMessage,
				webSocketChannel,
				[](bool error){},
				0x8A
			);

			// receive next message
			receiveMessage(
				webSocketChannel
			);
			return;
		}

		if (receiveMessageCallback_) receiveMessageCallback_(webSocketMessage);
		receiveMessage(webSocketChannel);
	}

	void
	WebSocketServerBase::idleTimeoutWebSocketChannel(WebSocketChannel* webSocketChannel, const std::string& location)
	{
		Log(Debug, "WebSocketServer idle timeout; close channel")
			.parameter("Address", webSocketChannel->partner_.address().to_string())
			.parameter("Port", webSocketChannel->partner_.port())
			.parameter("Location", location)
			.parameter("ChannelId", webSocketChannel->id_);

		webSocketChannel->timeout_ = true;
		webSocketChannel->socket().cancel();
	}

	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	//
	// handle send message
	//
	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	bool
	WebSocketServerBase::sendMessage(
		WebSocketMessage::SPtr& webSocketMessage,
		WebSocketChannel* webSocketChannel,
		const SendCompleteCallback& sendCompleteCallback,
		char headerByte
	)
	{
		// check send queue
		if (webSocketChannel->asyncWrite_ || !webSocketChannel->sendQueue_.empty()) {
			Log(Debug, "webSocketServer enqueue")
				.parameter("ChannelId", webSocketChannel->id_)
				.parameter("QueueSize", webSocketChannel->sendQueue_.size());
			webSocketChannel->sendQueue_.enqueue(
				webSocketMessage,
				sendCompleteCallback,
				headerByte
			);
			return true;
		}

		return sendMessageQueue(
			webSocketMessage,
			webSocketChannel,
			sendCompleteCallback,
			headerByte);
	}

	bool 
		WebSocketServerBase::sendMessageQueue(
			WebSocketMessage::SPtr & webSocketMessage,
			WebSocketChannel * webSocketChannel,
			const SendCompleteCallback & sendCompleteCallback,
			char headerByte)
	{
		std::ostream os(&webSocketChannel->sendBuffer_);

		char headerBytes[10];

		if (headerByte == 0x00) {
			// set FIN and text frame
			headerBytes[0] = (char)0x81;
		}
		else {
			headerBytes[0] = headerByte;
		}

		// set length
		uint32_t headerLength;
		uint64_t length = webSocketMessage->message_.length();
		if (length <= 125) {
			headerLength = 2;
			headerBytes[1] = (uint8_t)length;
		}
		else {
			if (length <= 0xFFFF) {

				headerLength = 4;
				headerBytes[1] = 126;

				char x;
				x = ((length >> (8 * 1)) % 256) & 0xFF;
				headerBytes[2] = x;
				x = (length % 256 >> (8 * 0)) & 0xFF;
				headerBytes[3] = x;
			}
			else {
				headerLength = 10;
				headerBytes[1] = 127;

				char x;
				x = (length % 256 >> (8 * 0)) & 0xFF;
				headerBytes[9] = x;
				x = ((length >> (8 * 1)) % 256) & 0xFF;
				headerBytes[8] = x;
				x = ((length >> (8 * 2)) % 256) & 0xFF;
				headerBytes[7] = x;
				x = ((length >> (8 * 3)) % 256) & 0xFF;
				headerBytes[6] = x;
				x = ((length >> (8 * 4)) % 256) & 0xFF;
				headerBytes[5] = x;
				x = ((length >> (8 * 5)) % 256) & 0xFF;
				headerBytes[4] = x;
				x = ((length >> (8 * 6)) % 256) & 0xFF;
				headerBytes[3] = x;
				x = ((length >> (8 * 7)) % 256) & 0xFF;
				headerBytes[2] = x;

			}

			for (uint32_t idx = 0; idx < headerLength - 2; idx++) {
				//headerBytes[idx+2] = 0;
			}
		}
		os.write(headerBytes, headerLength);

		// set message
		os.write(webSocketMessage->message_.c_str(), length);

		// send message
		SendCompleteCallback tmpSendCompleteCallback = sendCompleteCallback;
		webSocketChannel->asyncWrite_ = true;
		webSocketChannel->socket().async_write(
			webSocketConfig_->strand(),
			webSocketChannel->sendBuffer_,
			[this, tmpSendCompleteCallback, webSocketChannel](const boost::system::error_code& error, std::size_t bytes_transferred) {
			    handleWriteMessageComplete(error, bytes_transferred, webSocketChannel, tmpSendCompleteCallback);
			}
		);

		return true;
	}

	void
	WebSocketServerBase::handleWriteMessageComplete(
		const boost::system::error_code& error,
		std::size_t bytes_transferred,
		WebSocketChannel* webSocketChannel,
		const SendCompleteCallback& sendCompleteCallback
	)
	{
		Log(Debug, "WebSocketServer handle write complete")
			.parameter("ChannelId", webSocketChannel->id_)
			.parameter("QueueSize", webSocketChannel->sendQueue_.size());

		webSocketChannel->asyncWrite_ = false;

		if (error || bytes_transferred == 0 || webSocketChannel->shutdown_) {
			Log(Debug, "WebSocketServer send response error; close channel")
				.parameter("Address", webSocketChannel->partner_.address().to_string())
				.parameter("Port", webSocketChannel->partner_.port())
				.parameter("ChannelId", webSocketChannel->id_);

			sendCompleteCallback(false);
			closeWebSocketChannel(webSocketChannel);
			return;
		}

		sendCompleteCallback(true);

		// check send queue
		if (webSocketChannel->sendQueue_.empty()) {
			return;
		}

		// send the next message from send queue
		WebSocketMessage::SPtr webSocketMessage;
		SendCompleteCallback completeCallback;
		char headerByte;

		Log(Debug, "webSocketServer dequeue")
			.parameter("ChannelId", webSocketChannel->id_)
			.parameter("QueueSize", webSocketChannel->sendQueue_.size());
		webSocketChannel->sendQueue_.dequeue(
			webSocketMessage,
			&completeCallback,
		    &headerByte
		);

		sendMessageQueue(
			webSocketMessage,
			webSocketChannel,
			completeCallback,
			headerByte
		);		
	}

}
