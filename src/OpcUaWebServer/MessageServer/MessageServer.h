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

#ifndef __OpcUaWebServer_MessageManager_h__
#define __OpcUaWebServer_MessageManager_h__

#include <stdint.h>
#include <string>
#include "OpcUaStackCore/Base/Config.h"
#include "OpcUaWebServer/Messages/Message.h"
#include "OpcUaWebServer/MessageServer/MessageServerIf.h"

namespace OpcUaWebServer
{

	class MessageServer
	{
	  public:
		MessageServer(void);
		~MessageServer(void);

		bool startup(OpcUaStackCore::Config* config, MessageServerIf* messageServerIf);
		bool shutdown(void);
		void receiveMessage(uint32_t channelId, const std::string& message);
		void receiveMessage(Message::SPtr& message);

	  private:
		bool shutdown_ = false;
		OpcUaStackCore::Config* config_;
		MessageServerIf* messageServerIf_;
	};

}

#endif
