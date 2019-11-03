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

	SendQueueElement*
	SendQueueElement::last(void)
	{
		return last_;
	}

	void
	SendQueueElement::last(SendQueueElement* last)
	{
		last_ = last;
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
		return queue_ == nullptr;
	}

	void
	SendQueue::clear(void)
	{
		while (queue_ != nullptr) {
			auto queueEntry = queue_;

			if (queue_->last() == queue_->next()) {
				queue_ = nullptr;
			}
			else {
				queue_->last()->next(queue_->next());
				queue_->next()->last(queue_->last());
			}

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

		auto queueEntry = new SendQueueElement(
			webSocketMessage,
			sendCompleteCallback,
			headerByte
		);

		if (queue_ == nullptr) {
			queue_ = queueEntry;
			queueEntry->next(queueEntry);
			queueEntry->last(queueEntry);
		}
		else {
			queueEntry->next(queue_);
			queueEntry->last(queue_->last());
			queue_->last()->next(queueEntry);
			queue_->last(queueEntry);
		}

		return;
	}

	bool
	SendQueue::dequeue(
		WebSocketMessage::SPtr& webSocketMessage,
		std::function<void (bool error)>* sendCompleteCallback,
		char* headerByte
	)
	{
		if (queue_ == nullptr) {
			return false;
		}

		auto queueEntry = queue_;

		if (queue_->last() == queue_->next()) {
			queue_ = nullptr;
		}
		else {
			queue_->last()->next(queue_->next());
			queue_->next()->last(queue_->last());
		}

		webSocketMessage = queueEntry->webSocketMessage();
		*sendCompleteCallback = queueEntry->sendCompleteCallback();
		*headerByte = queueEntry->headerByte();

		delete queueEntry;

		size_--;
		return true;
	}

}
