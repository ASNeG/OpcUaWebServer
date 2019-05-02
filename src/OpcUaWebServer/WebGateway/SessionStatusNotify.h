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

#ifndef __OpcUaWebServer_SessionStatusNotify_h__
#define __OpcUaWebServer_SessionStatusNotify_h__

#include <boost/property_tree/ptree.hpp>

namespace OpcUaWebServer
{

	class SessionStatusNotify
	{
	  public:
		SessionStatusNotify(void);
		virtual ~SessionStatusNotify(void);

		std::string& sessionStatus(void);

		bool jsonEncode(boost::property_tree::ptree& pt);
		bool jsonDecode(boost::property_tree::ptree& pt);

	  private:
		std::string sessionStatus_;
	};

}

#endif
