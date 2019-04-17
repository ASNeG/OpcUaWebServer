/*
   Copyright 2015-2017 Kai Huebl (kai@huebl-sgh.de)

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

#ifndef __OpcUaWebSocket_WebSocketServer_h__
#define __OpcUaWebSocket_WebSocketServer_h__

#include "OpcUaStackCore/Base/ObjectPool.h"
#include "OpcUaWebServer/WebSocket/WebSocketServerBase.h"

using namespace OpcUaStackCore;

namespace OpcUaWebServer
{

	class WebSocketServer
	: public WebSocketServerBase
	, public Object
	{
	  public:
		typedef boost::shared_ptr<WebSocketServer> SPtr;

		WebSocketServer(WebSocketConfig* webSocketConfig);
		~WebSocketServer(void);

		bool startup(void);
		bool shutdown(void);

	  private:
		void accept(void);
		void handleAccept(const boost::system::error_code& error, WebSocketChannel* webSocketChannel);

		WebSocketConfig* webSocketConfig_;
	};

}

#endif
