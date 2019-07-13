Configuration
=============

ASNeG OPC UA Web Server has flexible configuration in XML format. In this document you'll
learn how to configure HTTP and WebSocket servers and describe data sources. The main configuration
file is **OpcUaWebServerModel.xml**. You can find in the sources:

::

  src/OpcUaWebServer/Config/OpcUaWebServerModel.xml

or in the direcotry where the server was installed (**@CONF_DIR**)

::
  
  path/to/directory/etc/OpcUaStack/OpcUaWebServer/OpcUaWebServerModel.xml


You can change the configuration in the sources if you want to distribute OpcUaWebServer with your configuration.


HTTP Server
-----------

The HTTP Server provides access to static web pages via HTTP protocol. Its configuration starts
with tag *HttpServer* in **OpcUaWebServerModel.xml**.

The default configuration:

  
.. code-block:: xml

  <HttpServer>
  	<Address>0.0.0.0</Address>
  	<Port>8080</Addreess>
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
| RequestTimeout                 | Time in milliseconds, that the client waits for an answer   |
|                                | before close the connection.                                |
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
