WebSocket Gateway
=================

The **WebSocket Gateway** provides a :term:`JSON API` via bidirectional WebSocket protocol for access to :term:`OPC UA` server :term:`Service`\ s.
Its configuration starts with XML tag *WebSocketGateway* in **OpcUaWebServerModel.xml**.

Default Configuration
---------------------

.. code-block:: xml

  <WebGateway>
      <Address>0.0.0.0</Address>
      <Port>8082</Port>
      <RequestTimeout>5000</RequestTimeout>
      <MaxConnections>100</MaxConnections>
      <IdleTimeout>3600000</IdleTimeout>

      <SSL>on</SSL>
      <CSRFile>@CONF_DIR@/ssl/crt/websocket.crt</CSRFile>
      <KeyFile>@CONF_DIR@/ssl/key/websocket.pem</KeyFile>
  </WebGateway>

Configuration Settings
----------------------

+--------------------------------+-------------------------------------------------------------+
| XML tag                        | Description                                                 |
+================================+=============================================================+
| IP Address                     | IP address bound by the WebSocket gateway                   |
+--------------------------------+-------------------------------------------------------------+
| Port                           | Port bound by the WebSocket gateway                         |
+--------------------------------+-------------------------------------------------------------+
| MaxConnections                 | Maximum number of simultaneous connections that this server |
|                                | supports.                                                   |
+--------------------------------+-------------------------------------------------------------+
| SSL                            | activates ("on") or deactivates ("off") the websocket       |
|                                | security. The SSL tag is optional. If the SSL tag is not    |
|                                | available, the websocket security is deactivated            |
+--------------------------------+-------------------------------------------------------------+
| CSRFile                        | Name of the certificate file. The tag must be specified     |
|                                | when the websocket security is switched on                  |
+--------------------------------+-------------------------------------------------------------+
| KeyFile                        | Name of the private key file. The tag must be specified     |
|                                | when the websocket security is switched on.                 |
+--------------------------------+-------------------------------------------------------------+
