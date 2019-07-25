ValueInfo
=========

Knowing the variable names the client can get the information about the configuration of the variables
by using *ValueInfo* request.

Request
-------

+------------------------------+----------------------------------------------------+
| Field                        | Description                                        | 
+============+=================+====================================================+
| **Header** |                 |                                                    |
+------------+-----------------+----------------------------------------------------+
|            | MessageType     | Should be *VALUEINFO_REQUEST*.                     |
+------------+-----------------+----------------------------------------------------+
|            | ClientHandler   | See :ref:`message_format`.                         |
+------------+-----------------+----------------------------------------------------+
| **Body**   |                 |                                                    |
+------------+-----------------+----------------------------------------------------+
|            | @Variables      | List of the variable names.                        |
+------------+-----------------+----------------------------------------------------+

Response
--------

+------------------------------+----------------------------------------------------+
| Field                        | Description                                        | 
+============+=================+====================================================+
| **Header** |                 |                                                    |
+------------+-----------------+----------------------------------------------------+
|            | MessageType     | Should be *VALUEINFO_RESPONSE*.                    |
+------------+-----------------+----------------------------------------------------+
|            | ClientHandler   | See :ref:`message_format`.                         |
+------------+-----------------+----------------------------------------------------+
| **Body**   |                 |                                                    |
+------------+-----------------+----------------------------------------------------+
|            | @Variables      | List of objects representing the configuration.    |
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
| BadAttributeInvalid   | There is no variable names in the request.                | 
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


