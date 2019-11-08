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

#include "OpcUaWebServer/WebSocket/SendQueue.h"

namespace OpcUaWebServer
{

	SendQueueElement::SendQueueElement(
		WebSocketMessage::SPtr& webSocketMessage,
		std::function<void (bool error)> sendCompleteCallback,
		char headerByte
	)
	: webSocketMessage_(webSocketMessage)
	, sendCompleteCallback_(sendCompleteCallback)
	, headerByte_(headerByte)
	{
	}

	SendQueueElement::~SendQueueElement(void)
	{
	}

	WebSocketMessage::SPtr
	SendQueueElement::webSocketMessage(void)
	{
		return webSocketMessage_;
	}

	std::function<void (bool error)>
	SendQueueElement::sendCompleteCallback(void)
	{
		return sendCompleteCallback_;
	}

	SendQueueElement*
	SendQueueElement::next(void)
	{
		return next_;
	}

	void
	SendQueueElement::next(SendQueueElement* next)
	{
		next_ = next;
	}

	char
	SendQueueElement::headerByte(void)
	{
		return headerByte_;
	}

	SendQueue::SendQueue(void)
	{
	}

	SendQueue::~SendQueue(void)
	{
	}

	bool
	SendQueue::empty(void)
	{
		return rear == nullptr;
	}

	void
	SendQueue::clear(void)
	{
		while (rear != nullptr) {
			auto queueEntry = front;
			front = front->next();

			if (front == nullptr)
				rear = nullptr;

			delete queueEntry;
		}
		size_ = 0;
	}

	uint32_t
	SendQueue::size(void)
	{
		return size_;
	}

	void
	SendQueue::enqueue(
		WebSocketMessage::SPtr& webSocketMessage,
		std::function<void (bool error)> sendCompleteCallback,
		char headerByte
	)
	{
		size_++;

		// Create a new LL entry 
		auto queueEntry = new SendQueueElement(
			webSocketMessage,
			sendCompleteCallback,
			headerByte
		);

		// If queue is empty, then 
		// new node is front and rear both 
		if (rear == nullptr) {
			front = rear = queueEntry;
			return;
		}

		// Add the new node at 
		// the end of queue and change rear 
		rear->next(queueEntry);
		rear = queueEntry;
	}

	bool
	SendQueue::dequeue(
		WebSocketMessage::SPtr& webSocketMessage,
		std::function<void (bool error)>* sendCompleteCallback,
		char* headerByte
	)
	{
		// If queue is empty, return NULL. 
		if (front == nullptr)
			return false;

		// Store previous front and 
		// move front one node ahead 
		auto queueEntry = front;
		front = front->next();
		webSocketMessage = queueEntry->webSocketMessage();
		*sendCompleteCallback = queueEntry->sendCompleteCallback();
		*headerByte = queueEntry->headerByte();

		delete queueEntry;

		// If front becomes NULL, then 
		// change rear also as NULL 
		if (front == nullptr)
			rear = nullptr;	

		size_--;
		return true;
	}

}