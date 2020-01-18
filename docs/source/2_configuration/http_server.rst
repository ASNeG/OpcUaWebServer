HTTP Server
===========

The **HTTP Server** provides access to static web pages via HTTP protocol. Its configuration starts
with tag *HttpServer* in **OpcUaWebServerModel.xml**.


Default Configuration
----------------------
  
.. code-block:: xml

  <HttpServer>
      <Address>0.0.0.0</Address>
      <Port>8080</Port>
      <WebDirectory>@CONF_DIR@/../../../var/www/OpcUaWebServer</WebDirectory>
      <MaxConnections>100</MaxConnections>
      <RequestTimeout>5000</RequestTimeout>
          
      <IPLogger>
      <LogFileName>@CONF_DIR@/../../../var/log/OpcUaStack/OpcUaWebServer/access.log</LogFileName>
      <MaxLogFileNumber>5</MaxLogFileNumber>
      <MaxLogFileSize>100000</MaxLogFileSize>
      <MaxIPAge>3600000</MaxIPAge>

      <SSL>on</SSL>
      <CSRFile>@CONF_DIR@/ssl/crt/websocket.crt</CSRFile>
      <KeyFile>@CONF_DIR@/ssl/key/websocket.pem</KeyFile>

    </IPLogger>
          
  </HttpServer>


Configuration Settings
----------------------

+--------------------------------+-------------------------------------------------------------+
| XML tag                        | Description                                                 |
+================================+=============================================================+
| IP Address                     | IP address bound by the HTTP server                         |
+--------------------------------+-------------------------------------------------------------+
| Port                           | Port bound by the HTTP server                               |
+--------------------------------+-------------------------------------------------------------+
| MaxConnections                 | Maximum number of simultaneous connections that this server |
|                                | supports.                                                   |
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
| SSL                            | activates ("on") or deactivates ("off") the http/https      |
|                                | protocol. The SSL tag is optional. If the SSL tag is not    |
|                                | available, the http protocol will be used                   |
+--------------------------------+-------------------------------------------------------------+
| CSRFile                        | Name of the certificate file. The tag must be specified     |
|                                | when the http protocol is switched on                       |
+--------------------------------+-------------------------------------------------------------+
| KeyFile                        | Name of the private key file. The tag must be specified     |
|                                | when the http protocol is switched on.                      |
+--------------------------------+-------------------------------------------------------------+


