Configuration
=============

ASNeG OPC UA Web Server has a flexible configuration in XML format. In this document, you'll
learn how to configure HTTP and WebSocket servers and describe data sources. 

The main configuration file is **OpcUaWebServerModel.xml**. You can find it in the sources:

::

  src/OpcUaWebServer/Config/OpcUaWebServerModel.xml

or in the directory where the server was installed (**@CONF_DIR**)

::
  
  path/to/directory/etc/OpcUaStack/OpcUaWebServer/OpcUaWebServerModel.xml


You can change the configuration in the sources if you want to distribute OpcUaWebServer with your configuration.


.. toctree::
   :maxdepth: 1

  http_server
  websocket_server
  websocket_gateway

