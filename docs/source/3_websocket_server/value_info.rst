.. _valueinfo:

Value Info
=========

Knowing the variable names the client can get the information about the configuration of the variables
by using *VALUEINFO_REQUEST* request.

.. _valueinfo_request:

Value Info Request
------------------

+------------------------------+----------------------------------------------------+
| Field                        | Description                                        | 
+============+=================+====================================================+
| **Header** |                 |                                                    |
+------------+-----------------+----------------------------------------------------+
|            | MessageType     | Must be *VALUEINFO_REQUEST*.                       |
+------------+-----------------+----------------------------------------------------+
|            | ClientHandler   | See :ref:`message_format`.                         |
+------------+-----------------+----------------------------------------------------+
| **Body**   |                 |                                                    |
+------------+-----------------+----------------------------------------------------+
|            | @Variables      | The list of the variable names.                    |
+------------+-----------------+----------------------------------------------------+

.. _valueinfo_response:

Value Info Response
-------------------

+------------------------------+----------------------------------------------------+
| Field                        | Description                                        | 
+============+=================+====================================================+
| **Header** |                 |                                                    |
+------------+-----------------+----------------------------------------------------+
|            | MessageType     | Must be *VALUEINFO_RESPONSE*.                      |
+------------+-----------------+----------------------------------------------------+
|            | ClientHandler   | See :ref:`message_format`.                         |
+------------+-----------------+----------------------------------------------------+
| **Body**   |                 |                                                    |
+------------+-----------------+----------------------------------------------------+
|            | @Variables      | The List of objects representing                   | 
|            |                 | the configuration.                                 |
+------------+---+-------------+----------------------------------------------------+
|            |   | [StatusCode]| The error occurs by getting the configuration.     |
+------------+---+-------------+----------------------------------------------------+
|            |   | Variable    | The name of the variable.                          |
+------------+---+-------------+----------------------------------------------------+
|            |   | Type        | The type of the variable.                          |
+------------+---+-------------+----------------------------------------------------+
|            |   | IsArray     | Equals 'true' if the variable is an array.         |
+------------+---+-------------+----------------------------------------------------+
|            |   | MetaData    | Additional information described in the            |
|            |   |             | configuration as metadata.                         |
+------------+---+-------------+----------------------------------------------------+

Status Codes
------------

+-----------------------+-----------------------------------------------------------+
| Status Code           | Description                                               |
+=======================+===========================================================+
| BadInternalError      | The server failed to process the request due to internal  |
|                       | error.                                                    |
+-----------------------+-----------------------------------------------------------+
| BadAttributeInvalid   | The server failed decode the body of the message.         | 
+-----------------------+-----------------------------------------------------------+
| BadNodeIdUnknown      | The variable name isn't found in the server configuration.| 
+-----------------------+-----------------------------------------------------------+

Example in Python
-----------------

.. code-block:: python

  import websocket
  import json

  msg = {
      'Header': {
        'MessageType':'VALUEINFO_REQUEST',
        'ClientHandle':'1'
      },
      'Body': { 'Variables' : ['Boolean']}
   }

 
  ws = websocket.create_connection('ws://127.0.0.1:8081')
  ws.send(json.dumps(msg)) 
  resp = ws.recv()  
  json.loads(resp)  #=> { 
                    # 'Header': {
                    #   'ClientHandle': '1', 
                    #   'MessageType': 'VALUEINFO_RESPONSE'
                    # },
                    # 
                    # 'Body': {
                    #   'Variables': [
                    #     {
                    #       'Variable': 'Boolean',
                    #       'Type': 'Boolean',
                    #       'IsArray': 'false',
                    #       'MetaData': {
                    #         'DisplayName': ' Switch',
                    #          'Limits': {'Max': ' 1', 'Min': ' 0'}
                    #          }
                    #     }
                    #   ] 
                    # }
                    #}


