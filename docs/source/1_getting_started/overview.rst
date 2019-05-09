Overview
====================

The ASNeG OPC UA Web Server provides a simple way to connect modern Web applications with the
OPC UA technology. Any process data can be displayed in realtime in Web applications using 
the ASNeG OPC UA Web Server. 

Structure of the OPC UA Web Server
-----------------------------------

The OpcUaWebServer contains the following components:

* HTTP Server
* Web Socket Server
* Web Gateway 
* OPC UA Client

.. figure:: opc_ua_web_server.png
   :scale: 100 %
   :alt: OpcUaWebServer Components
   :align: center


  **OpcUaWebServer components**

**HTTP Server** provides a simple interface for web pages. The provision of simple static web
pages in HTML format does not require a separate web server for simple web applications with
OPC UA access. The use of the the HTTP Server component is optional.

**Web Socket Server** provides a bidirectional web socket interface for access to OPC UA variables.
The Web Socket Server component is decoupled from the opc ua server. Symbolic names are used to
access OPC UA variables. A configuration in the Web Socket server maps these variables to OPC UA
variables and the assigned OPC UA server. The functionality of the interface is very simple but
limited to a few functions. The disadvantage of the interface is the high configuration effort. 
The use of the Web Socket Server component is optional. 

**Web Gateway** provides a bidirectional web socket interface for access to OPC UA server services.
The Web Socket Gateway component is not decoupled from the opc ua server. All web application JSON 
packets are transferred from the Web Gateway to binary OPC UA packets and sent directly to the 
assigned OPC UA server. A separate configuration for the Web Gateway is not necessary. As an advantage,
all OPC UA service functions of a OPC UA server can be used directly from the web application. However,
the JSON interface functions are a little bit complex than with the Web Socket Server model. The 
use of the Web Gateway component is optional.   


**OPC UA Client** is used for communikcation with one or more OPC UA servers.

