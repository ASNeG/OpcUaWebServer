/*
   Copyright 2019 Kai Huebl (kai@huebl-sgh.de)

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

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "OpcUaStackCore/Base/Log.h"
#include "OpcUaWebServer/WebGateway/ClientManager.h"
#include "OpcUaWebServer/WebGateway/ResponseHeader.h"

using namespace OpcUaStackCore;

namespace OpcUaWebServer
{

	ClientManager::ClientManager(void)
	: sendMessageCallback_()
	, disconnectChannelCallback_()
	{
	}

	ClientManager::~ClientManager(void)
	{
	}

	bool
	ClientManager::startup(
		IOThread::SPtr ioThread,
		CryptoManager::SPtr& cryptoManager
	)
	{
		return true;
	}

	bool
	ClientManager::shutdown(void)
	{
		return true;
	}

	void
	ClientManager::sendMessageCallback(const SendMessageCallback& sendMessageCallback)
	{
		sendMessageCallback_ = sendMessageCallback;
	}

	void
	ClientManager::disconnectChannelCallback(const DisconnectChannelCallback& disconnectChannelCallback)
	{
		disconnectChannelCallback_ = disconnectChannelCallback;
	}

	void
	ClientManager::receiveMessage(WebSocketMessage& webSocketMessage)
	{
		std::cout << "Command=" << webSocketMessage.command_ << std::endl;
		std::cout << "ChannelId=" << webSocketMessage.channelId_ << std::endl;
		std::cout << "ReceivedMessage=" << webSocketMessage.message_ << std::endl;

		boost::property_tree::ptree pt;
		std::stringstream ss;

		std::string errorMessage = "";
		bool error = false;

		// parse json message
		ss << webSocketMessage.message_;
		try {
			boost::property_tree::read_json(ss, pt);
		}
		catch (const boost::property_tree::json_parser_error& e)
		{
			errorMessage = std::string(e.what());
			error = true;
		}

		if (error) {
			Log(Error, "json parser error")
				.parameter("ChannelId", webSocketMessage.channelId_)
			    .parameter("Error", errorMessage);
			if (disconnectChannelCallback_) {
				disconnectChannelCallback_(webSocketMessage.channelId_);
			}
			return;
		}

		// get header from json message
		RequestHeader requestHeader;
		if (!requestHeader.jsonDecode(pt)) {
			Log(Error, "message header error");
			if (disconnectChannelCallback_) {
				disconnectChannelCallback_(webSocketMessage.channelId_);
			}
			return;
		}

		sendErrorResponse(webSocketMessage.channelId_, requestHeader, BadServiceUnsupported);
	}

	void
	ClientManager::sendResponse(
		uint32_t channelId,
		RequestHeader& requestHeader,
		std::string& body
	)
	{
		bool error = false;
		std::string errorMessage;
		boost::property_tree::ptree pt;

		// create header
		ResponseHeader responseHeader(requestHeader);
		responseHeader.jsonEncode(pt);

		// create body
		pt.put("Body", body);

		// create json message
		std::stringstream msg;
		try {
			boost::property_tree::write_json(msg, pt);
		}
		catch (const boost::property_tree::json_parser_error& e)
		{
			errorMessage = std::string(e.what());
			error = true;
		}

		if (error) {
			Log(Error, "json parser error")
			    .parameter("ChannelId", channelId)
			    .parameter("Error", errorMessage);
			return;
		}

		// create web socket message
		WebSocketMessage webSocketMessage;
		webSocketMessage.channelId_ = channelId;
		webSocketMessage.message_ = msg.str();

		if (sendMessageCallback_) {
			sendMessageCallback_(webSocketMessage);
		}
	}

	void
	ClientManager::sendErrorResponse(
		uint32_t channelId,
		RequestHeader& requestHeader,
		OpcUaStatusCode statusCode
	)
	{
		bool error = false;
		std::string errorMessage;
		boost::property_tree::ptree pt;

		// create header
		ResponseHeader responseHeader(requestHeader);
		responseHeader.jsonEncode(pt);

		// create json message
		std::stringstream msg;
		try {
			boost::property_tree::write_json(msg, pt);
		}
		catch (const boost::property_tree::json_parser_error& e)
		{
			errorMessage = std::string(e.what());
			error = true;
		}

		if (error) {
			Log(Error, "json parser error")
			    .parameter("ChannelId", channelId)
			    .parameter("Error", errorMessage);
			return;
		}

		// create web socket message
		WebSocketMessage webSocketMessage;
		webSocketMessage.channelId_ = channelId;
		webSocketMessage.message_ = msg.str();

		if (sendMessageCallback_) {
			sendMessageCallback_(webSocketMessage);
		}
	}

}

