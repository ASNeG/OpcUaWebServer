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



