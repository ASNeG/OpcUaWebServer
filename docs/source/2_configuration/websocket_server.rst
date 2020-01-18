WebSocket Server
================

The **WebSocket Server** provides a :term:`JSON API` via bidirectional :term:`WebSocket` protocol for access to :term:`OPC UA` variables.
Its configuration starts with XML tag *WebSocketServer* in **OpcUaWebServerModel.xml**.

Default Configuration
----------------------

.. code-block:: xml

  <WebSocketServer>
      <Address>0.0.0.0</Address>
      <Port>8081</Port>
      <MaxConnections>100</MaxConnections> 
      <RequestTimeout>5000</RequestTimeout>
      <IdleTimeout>3600000</IdleTimeout>

      <SSL>on</SSL>
      <CSRFile>@CONF_DIR@/ssl/crt/websocket.crt</CSRFile>
      <KeyFile>@CONF_DIR@/ssl/key/websocket.pem</KeyFile>
  </WebSocketServer>


Configuration Settings
----------------------

+--------------------------------+-------------------------------------------------------------+
| XML tag                        | Description                                                 |
+================================+=============================================================+
| IP Address                     | IP address bound by the WebSocket server                    |
+--------------------------------+-------------------------------------------------------------+
| Port                           | Port bound by the WebSocket server                          |
+--------------------------------+-------------------------------------------------------------+
| MaxConnections                 | Maximum number of simultaneous connections that this server |
|                                | supports.                                                   |
+--------------------------------+-------------------------------------------------------------+
| RequestTimeout                 | Time after the TCP connection establishment in milliseconds,|
|                                | that the server waits for the request                       |
|                                | from the client before closing the connection.              |
+--------------------------------+-------------------------------------------------------------+
| IdleTimeout                    | Time after the last message in milliseconds                 |
|                                | that the server waits before closing the connection.        |
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


OPC UA Client
-------------

In order to have access to :term:`OPC UA` servers we have to include its communication settings and nodes in **OpcUaClient** part 
into **OpcUaWebServerModel.xml** file:


.. code-block:: xml

  <OpcUaClient>
    <ClientConfigFile>@CONF_DIR@/OpcUaClient0.xml</ClientConfigFile>
    <ClientConfigFile>@CONF_DIR@/OpcUaClient1.xml</ClientConfigFile>
  </OpcUaClient>


Example of OpcUaClient Configuration File
`````````````````````````````````````````

.. code-block:: xml

  <?xml version="1.0" encoding="utf-8"?>
  <OpcUaClient Name="ASNeG-Demo_0" xmlns="http://ASNeG/OpcUaClient.xsd">
    <Endpoint>
      <ServerUri>opc.tcp://127.0.0.1:8889</ServerUri>
      <SecurityMode>SignAndEncrypt</SecurityMode>
      <SecurityPolicyUri>http://opcfoundation.org/UA/SecurityPolicy#Basic128Rsa15</SecurityPolicyUri>
      <UserAuth>
        <Type>UserName</Type>
        <UserName>user1</UserName>
        <Password>password1</Password>
        <SecurityPolicyUri>http://opcfoundation.org/UA/SecurityPolicy#Basic128Rsa15<SecurityPolicyUri>
      </UserAuth>
    </Endpoint>
    <NamespaceUri>
      <Uri>http://ASNeG-Demo.de/Test-Server-Lib/</Uri>
    </NamespaceUri>
    <NodeList>
      <Node ValueName="TimerInterval" NodeId="ns=1;i=3" NodeType="UInt32">
        <MetaData>
          <DisplayName>TimerInterval</DisplayName>
        </MetaData>
      </Node>
      <Node ValueName="Boolean" NodeId="ns=1;i=220" NodeType="Boolean">
        <MetaData>
          <DisplayName>Switch</DisplayName>
          <Limits>
            <Min>0</Min>
            <Max>1</Max>
          </Limits>
        </MetaData>
      </Node>
    </NodeList>
  </OpcUaClient>

.. note::
  You need client configuration files only for **WebSocket Server**. **WebSocket Gateway** receives the information about communication and security through :term:`JSON API`.

.. _opcuaclient_config:

OpcUaClient Configuration Settings
``````````````````````````````````

+--------------------------------+-------------------------------------------------------------+
| XML tag                        | Description                                                 |
+================================+=============================================================+
| Endpoint                       | :term:`Endpoint` of the :term:`OPC UA` Server, which        |
|                                | the client connect to                                       |  
+--------+-----------------------+-------------------------------------------------------------+
|        | ServerUri             | URI of the :term:`OPC UA` Server                            |
+--------+-----------------------+-------------------------------------------------------------+
|        | SecurityMode          | Security Mode can be "None", "Sign" and "SignAndCrypt".     |
|        |                       | Default value is "None".                                    |
+--------+-----------------------+-------------------------------------------------------------+
|        | SecurityPolicyUri     | Security Policy URI used to encrypt OPC UA messages.        |
|        |                       | See https://opcfoundation.org/UA/SecurityPolicy/            |
+--------+-----------------------+-------------------------------------------------------------+
|        | UserAuth              | Authentication settings, which the client use to connect    |
|        |                       | with the OPC UA server                                      |
+--------+------+----------------+-------------------------------------------------------------+
|        |      | Type           | Type of the authentication can be "Anonymous" or "UserName" |
+--------+------+----------------+-------------------------------------------------------------+
|        |      | Username       | Name of the authenticated user. Only for "Username" type.   |
+--------+------+----------------+-------------------------------------------------------------+
|        |      | Password       | Password of the authenticated user. Only for "Username"     |
|        |      |                | type.                                                       |
+--------+------+----------------+-------------------------------------------------------------+
|        |      | SecurityPolicy | Security Policy URI used to encrypt password. If it is      |
|        |      |                | empty the password is not encrypted.                        |
+--------+------+----------------+-------------------------------------------------------------+
| NamespaceUri                   | List of Namespace URIs                                      |
+--------+-----------------------+-------------------------------------------------------------+
|        | Uri                   | Namespace URI                                               |
+--------+-----------------------+-------------------------------------------------------------+
| NodeList                       | List of OPC UA :term:`Variable`\ s for access from          |
|                                | *WebSocket Server*.                                         |
+--------+-----------------------+-------------------------------------------------------------+
|        | Node                  | OPC UA :term:`Variable`\ s                                  |
+--------+------+----------------+-------------------------------------------------------------+
|        | Attr | ValueName      | Name of the variable to access with :term:`JSON API`        |               
+--------+------+----------------+-------------------------------------------------------------+
|        | Attr | NodeId         | ID of the corresponding :term:`Variable` in the OPCUA Server|               
+--------+------+----------------+-------------------------------------------------------------+
|        | Attr | ValueType      | Type of the OPC UA :term:`Variable`. Use OPC UA names       |
+--------+------+----------------+-------------------------------------------------------------+
|        | Attr | Array          | Equals 1 if the variable is an array                        |
+--------+------+----------------+-------------------------------------------------------------+
|        |      | MetaData       | Additional data that can be available through               |
|        |      |                | :term:`JSON API`.                                           |
+--------+------+----------------+-------------------------------------------------------------+

WebSocket Gateway
=================

The **WebSocket Server** provides a :term:`JSON API` via bidirectional :term:`WebSocket` protocol for access to :term:`OPC UA` variables.
Its configuration starts with XML tag *WebSocketServer* in **OpcUaWebServerModel.xml**.

Default Configuration
----------------------

.. code-block:: xml

  <WebSocketServer>
      <Address>0.0.0.0</Address>
      <Port>8081</Port>
      <RequestTimeout>5000</RequestTimeout>
      <IdleTimeout>3600000</IdleTimeout>

      <SSL>on</SSL>
      <CSRFile>@CONF_DIR@/ssl/crt/websocket.crt</CSRFile>
      <KeyFile>@CONF_DIR@/ssl/key/websocket.pem</KeyFile>
  </WebSocketServer>


Configuration Settings
----------------------

+--------------------------------+-------------------------------------------------------------+
| XML tag                        | Description                                                 |
+================================+=============================================================+
| IP Address                     | IP address bound by the WebSocket server                    |
+--------------------------------+-------------------------------------------------------------+
| Port                           | Port bound by the WebSocket server                          |
+--------------------------------+-------------------------------------------------------------+
| RequestTimeout                 | Time after the TCP connection establishment in milliseconds,|
|                                | that the server waits for the request                       |
|                                | from the client before closing the connection.              |
+--------------------------------+-------------------------------------------------------------+
| IdleTimeout                    | Time after the last message in milliseconds                 |
|                                | that the server waits before closing the connection.        |
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



.. note:: *NodeId* has the format common for OPC UA standard (e.g. "i=208;ns=0"), but be
  careful! "ns" means the namsespace index in *NapespaceUri* list not the name space of the server.

