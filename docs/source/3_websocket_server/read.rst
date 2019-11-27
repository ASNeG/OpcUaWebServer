.. _read:

Read Value
==========

To read the value of a variable, the client must use *READ_REQUEST* request.

.. _read_request:

Read Request
------------

+------------------------------+----------------------------------------------------+
| Field                        | Description                                        | 
+============+=================+====================================================+
| **Header** |                 |                                                    |
+------------+-----------------+----------------------------------------------------+
|            | MessageType     | Must be *READ_REQUEST*.                            |
+------------+-----------------+----------------------------------------------------+
|            | ClientHandler   | See :ref:`message_format`.                         |
+------------+-----------------+----------------------------------------------------+
| **Body**   |                 |                                                    |
+------------+-----------------+----------------------------------------------------+
|            | Variable        | The variable to read.                              |
+------------+-----------------+----------------------------------------------------+

.. _read_response:

Read Response
--------------

+------------------------------+----------------------------------------------------+
| Field                        | Description                                        | 
+============+=================+====================================================+
| **Header** |                 |                                                    |
+------------+-----------------+----------------------------------------------------+
|            | MessageType     | Must be *READ_RESPONSE*.                           |
+------------+-----------------+----------------------------------------------------+
|            | ClientHandler   | See :ref:`message_format`.                         |
+------------+-----------------+----------------------------------------------------+
| **Body**   |                 |                                                    |
+------------+-----------------+----------------------------------------------------+
|            | Value           |                                                    |
+------------+---+-------------+----------------------------------------------------+
|            |   | Body        | The value of the variable.                         |
+------------+---+-------------+----------------------------------------------------+
|            |   | Type        | The type of the variable.                          |
+------------+---+-------------+----------------------------------------------------+
|            | [Status]        | The OPC UA status of the variable                  |
|            |                 | if it is not *Success*.                            |
+------------+-----------------+----------------------------------------------------+
|            | SourceTimestamp | The time of the value given by the source in ISO   |
|            |                 | 8601 format. Example: "2015-09-06T09:03:21Z"       |
+------------+-----------------+----------------------------------------------------+
|            | ServerTimestamp | The time of the value given by the server in ISO   |
|            |                 | 8601 format. Example: "2015-09-06T09:03:21Z"       |
+------------+-----------------+----------------------------------------------------+

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
| BadSessionClosed      | The connection with OPC UA server is lost.                | 
+-----------------------+-----------------------------------------------------------+

Example in Python
-----------------

.. code-block:: python

  import websocket
  import json

  msg = {
      'Header': {
        'MessageType':'READ_REQUEST',
        'ClientHandle':'1'
      },
      'Body': { 'Variable' : 'Boolean'}
   }

  ws = websocket.create_connection('ws://127.0.0.1:8081')
  ws.send(json.dumps(msg)) 
  resp = ws.recv()  
  json.loads(resp)  #=> {
                    # "Header": {
                    #    "MessageType": "READ_RESPONSE",
                    #    "ClientHandle": "1"
                    # },
                    # "Body": {
                    #    "Value": {
                    #        "Type": 1,
                    #        "Body": true
                    #    },
                    #    "SourceTimestamp": "2019-07-26T11:10:20Z",
                    #    "ServerTimestamp": "2019-07-26T11:10:20Z"
                    # }
                    #}


