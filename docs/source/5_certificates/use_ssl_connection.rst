Use SSL connection
===================

In addition to the OPC UA protocol, the OpcUaWebServer uses the http protocol and the ws 
protocol for communication. The https protocol and the wss protocol can also be used for
secure communication. In this case, the user must provide a private key and a certificate 
for the OpcUaWebServer. 

The certificate can be signed by yourself or authorized by a CA (Certificate Authority).
If a selg-signed certificate is used, a exeption must be created by the user for the
browser when it is accessed. 

To do this, type the following into the browser and create an exception.

::

  https://<IP-Address>:8080

::

  https://<IP-Address>:8081

::

  https://<IP-Address>:8082
