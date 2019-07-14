Configuration
=============

ASNeG OPC UA Web Server has a flexible configuration in XML format. In this document, you'll
learn how to configure HTTP and WebSocket servers and describe data sources. The main configuration
file is **OpcUaWebServerModel.xml**. You can find in the sources:

::

  src/OpcUaWebServer/Config/OpcUaWebServerModel.xml

or in the directory where the server was installed (**@CONF_DIR**)

::
  
  path/to/directory/etc/OpcUaStack/OpcUaWebServer/OpcUaWebServerModel.xml


You can change the configuration in the sources if you want to distribute OpcUaWebServer with your configuration.


HTTP Server
-----------

The **HTTP Server** provides access to static web pages via HTTP protocol. Its configuration starts
with tag *HttpServer* in **OpcUaWebServerModel.xml**.

The default configuration:

  
.. code-block:: xml

  <HttpServer>
      <Address>0.0.0.0</Address>
      <Port>8080</Port>
      <WebDirectory>@CONF_DIR@/../../../var/www/OpcUaWebServer</WebDirectory>
      <RequestTimeout>5000</RequestTimeout>
          
      <IPLogger>
      <LogFileName>@CONF_DIR@/../../../var/log/OpcUaStack/OpcUaWebServer/access.log</LogFileName>
      <MaxLogFileNumber>5</MaxLogFileNumber>
      <MaxLogFileSize>100000</MaxLogFileSize>
      <MaxIPAge>3600000</MaxIPAge>
    </IPLogger>
          
  </HttpServer>


The configuration has the following settings:

+--------------------------------+-------------------------------------------------------------+
| XML tag                        | Description                                                 |
+================================+=============================================================+
| IP Address                     | IP address bound by the HTTP server                         |
+--------------------------------+-------------------------------------------------------------+
| Port                           | Port bound by the HTTP server                               |
+--------------------------------+-------------------------------------------------------------+
| WebDirectory                   | Root directory. It should have *index.html* file.           |
+--------------------------------+-------------------------------------------------------------+
| RequestTimeout                 | Time after the TCP connection establishment in milliseconds,|
|                                | that the server waits for the request                       |
|                                | from the client before closing the connection.              |
+----------+---------------------+-------------------------------------------------------------+
| IPLogger |                     | IP Logger registers all IP addresses of the clients that    |
|          |                     | have connected with the server.                             |
+----------+---------------------+-------------------------------------------------------------+
|          | LogFileName         | Full file name of the log                                   |
+----------+---------------------+-------------------------------------------------------------+
|          | MaxLogFileSize      | Max. count of the log files                                 |
+----------+---------------------+-------------------------------------------------------------+
|          | MaxLogFileSize      | Max. size of the log files in bytes                         |   
+----------+---------------------+-------------------------------------------------------------+
|          | MaxIPAge            | If the client connects to the server several times          |
|          |                     | during this period in milliseconds, only one record         |
|          |                     | is written in the log.                                      | 
+----------+---------------------+-------------------------------------------------------------+


WebSocket Server
----------------

The **WebSocket Server** provides a :term:`JSON API` via bidirectional :term:`WebSocket` protocol for access to :term:`OPC UA` variables.
Its configuration starts with XML tag *WebSocketServer* in **OpcUaWebServerModel.xml**.

The default configuration:

  
.. code-block:: xml

  <WebSocketServer>
      <Address>0.0.0.0</Address>
      <Port>8081</Port>
      <RequestTimeout>5000</RequestTimeout>
      <IdleTimeout>3600000</IdleTimeout>
  </WebSocketServer>

The configuration has the following settings:

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


WebSocket Gateway
-----------------

The **WebSocket Gateway** provides a :term:`JSON API` via bidirectional WebSocket protocol for access to :term:`OPC UA` server :term:`Service`\ s.
Its configuration starts with XML tag *WebSocketGateway* in **OpcUaWebServerModel.xml**.

The default configuration:

.. code-block:: xml

  <WebGateway>
    <Address>0.0.0.0</Address>
      <Port>8082</Port>
  </WebGateway>

The configuration has the following settings:

+--------------------------------+-------------------------------------------------------------+
| XML tag                        | Description                                                 |
+================================+=============================================================+
| IP Address                     | IP address bound by the WebSocket gateway                   |
+--------------------------------+-------------------------------------------------------------+
| Port                           | Port bound by the WebSocket gateway                         |
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


An example of the client configuration file:

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

The **OpcUaClient** configuration has the following format:

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



