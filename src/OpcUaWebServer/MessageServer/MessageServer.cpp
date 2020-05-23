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

#include <iostream>
#include <boost/make_shared.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "OpcUaStackCore/Base/Log.h"
#include "OpcUaWebServer/MessageServer/MessageServer.h"

using namespace OpcUaStackCore;

namespace OpcUaWebServer
{

	MessageServer::MessageServer(void)
	: config_(nullptr)
	, messageServerIf_(nullptr)
	{
	}

	MessageServer::~MessageServer(void)
	{
	}

	bool
	MessageServer::startup(Config* config, MessageServerIf* messageServerIf)
	{
		config_ = config;
		messageServerIf_ = messageServerIf;
		return true;
	}

	bool
	MessageServer::shutdown(void)
	{
		shutdown_ = true;
		return true;
	}

	void
	MessageServer::receiveMessage(uint32_t channelId, const std::string& message)
	{
		if (shutdown_) {
			return;
		}

		boost::property_tree::ptree pt;
		std::stringstream ss;

		std::string errorMessage = "";
		bool error = false;

		// parse json message
		ss << message;
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
			    .parameter("ChannelId", channelId)
			    .parameter("Error", errorMessage);
			return;
		}

		// get client handle from json message
		boost::optional<std::string> clientHandle = pt.get<std::string>("Header.ClientHandle");
		if (!clientHandle) {
			Log(Error, "message header do not contain client handle")
				.parameter("ChannelId", channelId);
			return;
		}

		// get message type from json message
		boost::optional<std::string> messageTypeString = pt.get<std::string>("Header.MessageType");
		if (!messageTypeString) {
			Log(Error, "message header do not contain message type")
				.parameter("ChannelId", channelId);
			return;
		}
		Message::MessageType messageType = Message::string2MessageType(*messageTypeString);
		if (messageType == Message::MT_Unknown) {
			Log(Error, "message type unknown")
			    .parameter("ChannelId", channelId)
			    .parameter("MessageType", *messageTypeString);

		}

		// get message body
		boost::optional<boost::property_tree::ptree&> body = pt.get_child_optional("Body");
		if (!body) {
			Log(Error, "message do not contain message body")
				.parameter("ChannelId", channelId);
			return;
		}

		// create message
		auto msg = boost::make_shared<Message>(messageType);
		msg->channelId(channelId);
		msg->clientHandle(*clientHandle);
		msg->body(*body);

		messageServerIf_->messageServerMessage(msg);
	}

	void
	MessageServer::receiveMessage(Message::SPtr& message)
	{
		boost::property_tree::ptree pt;
		pt.put("Header.MessageType", message->messageTypeString());
		pt.put("Header.ClientHandle", message->clientHandle());
		if (message->statusCode().size() != 0) pt.put("Header.StatusCode", message->statusCode());
		if (message->statusCode().size() == 0) pt.add_child("Body", message->body());

		std::string errorMessage = "";
		bool error = false;

		// write json message
		std::stringstream ss;
		try {
			boost::property_tree::write_json(ss, pt);
		}
		catch (const boost::property_tree::json_parser_error& e)
		{
			errorMessage = std::string(e.what());
			error = true;
		}

		if (error) {
			Log(Error, "json parser error")
			    .parameter("ChannelId", message->channelId())
			    .parameter("Error", errorMessage);
			return;
		}

		std::cout << "Output: " << ss.str() << std::endl;
		messageServerIf_->messageServerMessage(message->channelId(), ss.str());
	}

}
