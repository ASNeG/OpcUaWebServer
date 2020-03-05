.. _session:

Session
=======

**WebSocket Gateway** processes any requests only in context of OPCUA session between the gateway and OPC UA Server.
So you should request to open the session before starting the communication with the gateway and kindly
close it after the communication has been ended.


.. _gw_login_request:

GW_LoginRequest
---------------

Opens a session with an OPC UA Server by using *DiscoveryUrl*.

+----------------------------------+------------------------------------------------------------------+
| Field                            | Description                                                      |
+============+=====================+==================================================================+
| **Header** |                     |                                                                  |
+------------+---------------------+------------------------------------------------------------------+
|            | MessageType         | Must be *GW_LoginRequest*.                                       |
+------------+---------------------+------------------------------------------------------------------+
|            | ClientHandler       | See :ref:`gw_message_format`.                                    |
+------------+---------------------+------------------------------------------------------------------+
| **Body**   |                     |                                                                  |
+------------+---------------------+------------------------------------------------------------------+
|            | DiscoveryUrl        | The network address                                              |
+------------+---------------------+------------------------------------------------------------------+
|            | [SecurityPolicyUri] | The URI for SecurityPolicy to use when securing messages:        |
|            |                     |   "http://opcfoundation.org/UA/SecurityPolicy#None" (default)    |
|            |                     |   "http://opcfoundation.org/UA/SecurityPolicy#Basic128Rsa15"     |
|            |                     |   "http://opcfoundation.org/UA/SecurityPolicy#Basic256"          |
|            |                     |   "http://opcfoundation.org/UA/SecurityPolicy#Basic256Sha256"    |
+------------+---------------------+------------------------------------------------------------------+
|            | [SecurityMode]      | The type of security to apply to the messages:                   |
|            |                     |   "None" (default)                                               |
|            |                     |   "Sign"                                                         |
|            |                     |   "SignAndEncrypt"                                               |
+------------+---------------------+------------------------------------------------------------------+
|            | [UserAuth]          | Authentication data                                              |
|            +--+------------------+------------------------------------------------------------------+
|            |  |  PolicyId        | The type name of the identity tokens:                            |
|            |  |                  |    "Anonymous" (default)                                         |
|            |  |                  |    "UserName"                                                    |
|            |  |                  |    "Certificate"                                                 |
|            |  |                  |    "Issued"                                                      |
|            +--+------------------+------------------------------------------------------------------+
|            |  | [*]              | *UserAuth* has various fields that depends on the identity token |
|            |  |                  | See the tables below.                                            |
+------------+--+------------------+------------------------------------------------------------------+

**Anonymous Identity Token**

The Anonymous Identity Token is used to indicate that the Client has no user credentials.

+-------------------------------------------+------------------------------------------------------------------+
| Field                                     | Description                                                      |
+======================+====================+==================================================================+
| **UserAuth**         | PolicyId           | Must be "Anonymous"                                              |
+----------------------+--------------------+------------------------------------------------------------------+

**UserName Identity Token**

The UserNameIdentityToken is used to pass simple username/password credentials to the Server.

+---------------------------------------------+------------------------------------------------------------------+
| Field                                       | Description                                                      |
+======================+======================+==================================================================+
| **UserAuth**         | PolicyId             | Must be "UserName"                                               |
+----------------------+----------------------+------------------------------------------------------------------+
|                      | UserName             | A string that identifies the user.                               |
+----------------------+----------------------+------------------------------------------------------------------+
|                      | Password             | The password for the user.                                       |
+----------------------+----------------------+------------------------------------------------------------------+
|                      | [SecurityPolicyUri]  | The policy how to encrypt the password:                          |
|                      |                      |   "http://opcfoundation.org/UA/SecurityPolicy#None" (default)    |
|                      |                      |   "http://opcfoundation.org/UA/SecurityPolicy#Basic128Rsa15"     |
|                      |                      |   "http://opcfoundation.org/UA/SecurityPolicy#Basic256"          |
|                      |                      |   "http://opcfoundation.org/UA/SecurityPolicy#Basic256Sha256"    |
+----------------------+----------------------+------------------------------------------------------------------+

**Certificate Identity Token**

The CertificateIdentityToken is used to pass an X.509 v3 Certificate which is issued by the user
(see 7.36.5 *X509IdentityTokens*  in OPCUA Specification Part 4, Services).

+---------------------------------------------+------------------------------------------------------------------+
| Field                                       | Description                                                      |
+======================+======================+==================================================================+
| **UserAuth**         | PolicyId             | Must be "Certificate"                                            |
+----------------------+----------------------+------------------------------------------------------------------+
|                      | Certificate          | The X.509 v3 Certificate in DER format                           |
+----------------------+----------------------+------------------------------------------------------------------+
|                      | PrivateKey           | The web client's private keys. It is needed to sign the          |
|                      |                      | certificate on the web server side.                              |
+----------------------+----------------------+------------------------------------------------------------------+
|                      | [SecurityPolicyUri]  | The policy to sign the certificate:                              |
|                      |                      |   "http://opcfoundation.org/UA/SecurityPolicy#None" (default)    |
|                      |                      |   "http://opcfoundation.org/UA/SecurityPolicy#Basic128Rsa15"     |
|                      |                      |   "http://opcfoundation.org/UA/SecurityPolicy#Basic256"          |
|                      |                      |   "http://opcfoundation.org/UA/SecurityPolicy#Basic256Sha256"    |
+----------------------+----------------------+------------------------------------------------------------------+

**Issued Identity Token**

The IssuedIdentityToken is used to pass SecurityTokens issued by an external Authorization
Service to the Server. These tokens may be text or binary.

+---------------------------------------------+------------------------------------------------------------------+
| Field                                       | Description                                                      |
+======================+======================+==================================================================+
| **UserAuth**         | PolicyId             | Must be "Certificate"                                            |
+----------------------+----------------------+------------------------------------------------------------------+
|                      | tokenData            | The text or binary representation of the token.                  |
+----------------------+----------------------+------------------------------------------------------------------+
|                      | [SecurityPolicyUri]  | The policy to encrypt the token:                                 |
|                      |                      |   "http://opcfoundation.org/UA/SecurityPolicy#None" (default)    |
|                      |                      |   "http://opcfoundation.org/UA/SecurityPolicy#Basic128Rsa15"     |
|                      |                      |   "http://opcfoundation.org/UA/SecurityPolicy#Basic256"          |
|                      |                      |   "http://opcfoundation.org/UA/SecurityPolicy#Basic256Sha256"    |
+----------------------+----------------------+------------------------------------------------------------------+

.. _gw_login_response:

GW_LoginResponse
----------------

+----------------------------------+------------------------------------------------------------------+
| Field                            | Description                                                      |
+============+=====================+==================================================================+
| **Header** |                     |                                                                  |
+------------+---------------------+------------------------------------------------------------------+
|            | MessageType         | Must be *GW_LoginResponse*.                                      |
+------------+---------------------+------------------------------------------------------------------+
|            | ClientHandler       | See :ref:`gw_message_format`.                                    |
+------------+---------------------+------------------------------------------------------------------+
| **Body**   |                     |                                                                  |
+------------+---------------------+------------------------------------------------------------------+
|            | SessionId           | The unique ID of OPC UA session                                  |
+------------+---------------------+------------------------------------------------------------------+

.. _gw_session_status_notify:

GW_SessionStatusNotify
----------------------

**WebSocket Gateway** notifies the clients if the connection has changed its status by sending a special JSON message
asynchronously.

+----------------------------------+------------------------------------------------------------------+
| Field                            | Description                                                      |
+============+=====================+==================================================================+
| **Header** |                     |                                                                  |
+------------+---------------------+------------------------------------------------------------------+
|            | MessageType         | Must be *GW_SessionStatusNotify*.                                |
+------------+---------------------+------------------------------------------------------------------+
|            | ClientHandler       | See :ref:`gw_message_format`.                                    |
+------------+---------------------+------------------------------------------------------------------+
|            | SessionId           | The ID of the current session                                    |
+------------+---------------------+------------------------------------------------------------------+
| **Body**   |                     |                                                                  |
+------------+---------------------+------------------------------------------------------------------+
|            | SessionStatus       | Can be "Connect" or "Disconnect"                                 |
+------------+---------------------+------------------------------------------------------------------+

.. _gw_logout_request:

GW_LogoutRequest
----------------

The client should request closing OPCUA session by sending *GW_LogoutRequest* request

+----------------------------------+------------------------------------------------------------------+
| Field                            | Description                                                      |
+============+=====================+==================================================================+
| **Header** |                     |                                                                  |
+------------+---------------------+------------------------------------------------------------------+
|            | MessageType         | Must be *GW_LogoutRequest*.                                      |
+------------+---------------------+------------------------------------------------------------------+
|            | ClientHandler       | See :ref:`gw_message_format`.                                    |
+------------+---------------------+------------------------------------------------------------------+
|            | SessionId           | The ID of the current session                                    |
+------------+---------------------+------------------------------------------------------------------+
| **Body**   |                     |                                                                  |
+------------+---------------------+------------------------------------------------------------------+


.. _gw_logout_response:

GW_LogoutResponse
-----------------

+----------------------------------+------------------------------------------------------------------+
| Field                            | Description                                                      |
+============+=====================+==================================================================+
| **Header** |                     |                                                                  |
+------------+---------------------+------------------------------------------------------------------+
|            | MessageType         | Must be *GW_LogoutResponse*.                                     |
+------------+---------------------+------------------------------------------------------------------+
|            | ClientHandler       | See :ref:`gw_message_format`.                                    |
+------------+---------------------+------------------------------------------------------------------+
|            | SessionId           | The ID of the current session                                    |
+------------+---------------------+------------------------------------------------------------------+
| **Body**   |                     |                                                                  |
+------------+---------------------+------------------------------------------------------------------+


.. _gw_session_example:

Handling OPCUA Session in Python
--------------------------------

.. code-block:: python

  import websocket
  import json

  open_session_request = {
            "Header": {
                "MessageType": "GW_LoginRequest",
                "ClientHandle": "client-handle"
            },
            "Body": {
                "DiscoveryUrl": "opc.tcp://127.0.0.1:4840",
                "SecurityMode": "SignAndEncrypt",
                "SecurityPolicyUri": "http://opcfoundation.org/UA/SecurityPolicy#Basic128Rsa15",
                "UserAuth": {
                    "Type": "UserName",
                    "UserName": "user1",
                    "Password": "password1",
                    "SecurityPolicyUri": "http://opcfoundation.org/UA/SecurityPolicy#Basic128Rsa15"
                }
            }
        }


  ws = websocket.create_connection('ws://127.0.0.1:8082")
  ws.send(json.dumps(open_session_request))
  resp = ws.recv()
  json.loads(resp)  #=> {
                    # "Header": {
                    #   "MessageType": "GW_LoginResponse",
                    #   "ClientHandle": "client-handle",
                    #   "SessionId": "",
                    #   "StatusCode": "0"
                    # },
                    # "Body": {
                    #   "SessionId": "1"
                    #   }
                    # }
  resp = ws.recv()
  json.loads(resp)  # => {
                    # "Header": {
                    #   "MessageType": "GW_SessionStatusNotify",
                    #   "ClientHandle": "client-handle",
                    #   "SessionId": "1"
                    # },
                    #   "Body": {
                    #   "SessionStatus": "Connect"
                    # }
                    #}

  close_session_request = {
            "Header": {
                "MessageType": "GW_LogoutRequest",
                "ClientHandle": "client-handle",
                "SessionId": sessionId
            },
            "Body": {
            }
        }

  ws.send(json.dumps(close_session_request))

  resp = ws.recv()
  json.loads(resp) #=> {
                   # "Header": {
                   #    "MessageType": "GW_LogoutResponse",
                   #    "ClientHandle": "client-handle",
                   #    "SessionId": "1",
                   #    "StatusCode": "0"
                   # },
                   # "Body": ""
                   # }



