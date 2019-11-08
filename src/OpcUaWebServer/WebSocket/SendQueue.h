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

#ifndef __OpcUaWebServer_SendQueue_h__
#define __OpcUaWebServer_SendQueue_h__

#include <boost/shared_ptr.hpp>
#include "OpcUaWebServer/WebSocket/WebSocketMessage.h"

namespace OpcUaWebServer
{

	class SendQueueElement
	{
	  public:
		typedef boost::shared_ptr<SendQueueElement> SPtr;

		SendQueueElement(
			WebSocketMessage::SPtr& webSocketMessage,
			std::function<void (bool error)> sendCompleteCallback,
			char headerByte
		);
		~SendQueueElement(void);

		WebSocketMessage::SPtr webSocketMessage(void);
		std::function<void (bool error)> sendCompleteCallback(void);
		char headerByte(void);

		SendQueueElement* next(void);
		void next(SendQueueElement* next);

	  private:
		SendQueueElement* next_ = nullptr;

		WebSocketMessage::SPtr webSocketMessage_;
		std::function<void (bool error)> sendCompleteCallback_ = nullptr;
		char headerByte_ = 0x00;
	};

	class SendQueue
	{
	  public:
		typedef boost::shared_ptr<SendQueue> SPtr;

		SendQueue(void);
		~SendQueue(void);

		bool empty(void);
		void clear(void);
		uint32_t size(void);
		void enqueue(
			WebSocketMessage::SPtr& webSocketMessage,
			std::function<void (bool error)> sendCompleteCallback,
			char headerByte
		);

		bool dequeue(
			WebSocketMessage::SPtr& webSocketMessage,
			std::function<void (bool error)>* sendCompleteCallback,
			char* headerByte
		);

	  private:
		uint32_t size_ = 0;
		SendQueueElement* front = nullptr;
		SendQueueElement* rear = nullptr;
	};

}

#endif
