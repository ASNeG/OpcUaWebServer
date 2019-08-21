/*
   Copyright 2015-2019 Kai Huebl (kai@huebl-sgh.de)

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

#ifndef __OpcUaWebServer_Message_h__
#define __OpcUaWebServer_Message_h__

#include <boost/shared_ptr.hpp>
#include <boost/property_tree/ptree.hpp>
#include <map>
#include <stdint.h>
#include "OpcUaStackCore/Base/Object.h"

namespace OpcUaWebServer
{

	class Message
	: public OpcUaStackCore::Object
	{
	  public:
		typedef boost::shared_ptr<Message> SPtr;

		typedef enum
		{
			MT_Unknown,
			MT_Error,
			MT_ReadRequest,
			MT_ReadResponse,
			MT_WriteRequest,
			MT_WriteResponse,
			MT_ValueListRequest,
			MT_ValueListResponse,
			MT_ValueInfoRequest,
			MT_ValueInfoResponse,
			MT_MonitorStartRequest,
			MT_MonitorStartResponse,
			MT_MonitorStopRequest,
			MT_MonitorStopResponse,
			MT_MonitorUpdateMessage,
			MT_HistorcialReadRequest,
			MT_HistoricalReadResponse,
			MT_ChannelCloseMessage

		} MessageType;

		static MessageType string2MessageType(const std::string& messageType);

		Message(MessageType messageType);
		virtual ~Message(void);

		std::string messageTypeString(void);
		MessageType messageType(void);
		void channelId(uint32_t channelId);
		uint32_t channelId(void);
		void clientHandle(const std::string& clientHandle);
		std::string& clientHandle(void);
		void statusCode(const std::string& statusCode);
		std::string& statusCode(void);
		void body(boost::property_tree::ptree& body);
		boost::property_tree::ptree& body(void);


	  protected:
		MessageType messageType_;
		uint32_t channelId_;
		std::string clientHandle_;
		std::string statusCode_;

		boost::property_tree::ptree body_;
	};

}

#endif
