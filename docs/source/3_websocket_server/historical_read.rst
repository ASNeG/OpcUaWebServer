.. _hisread:

Historical Read
===============

To read the historical values of a variable, the client must use *HISTORICALREAD_REQUEST* request.

.. _hisread_request:

Historical Read Request
-----------------------

+------------------------------+----------------------------------------------------+
| Field                        | Description                                        | 
+============+=================+====================================================+
| **Header** |                 |                                                    |
+------------+-----------------+----------------------------------------------------+
|            | MessageType     | Must be *HISTORICALREAD_REQUEST*.                  |
+------------+-----------------+----------------------------------------------------+
|            | ClientHandler   | See :ref:`message_format`.                         |
+------------+-----------------+----------------------------------------------------+
| **Body**   |                 |                                                    |
+------------+-----------------+----------------------------------------------------+
|            | Variable        | The variable to read                               |
+------------+-----------------+----------------------------------------------------+
|            | StartTime       | Beginning of period to read in ISO                 |
|            |                 | 8601 format. Example: "2015-09-06T09:03:21Z"       |
+------------+-----------------+----------------------------------------------------+
|            | Endtime         | End of period to read in ISO                       |
|            |                 | 8601 format. Example: "2015-09-06T09:03:21Z"       |
+------------+-----------------+----------------------------------------------------+


.. _hisread_response:

Historical Read Response
------------------------

+---------------------------------+----------------------------------------------------+
| Field                           | Description                                        | 
+============+====================+====================================================+
| **Header** |                    |                                                    |
+------------+--------------------+----------------------------------------------------+
|            | MessageType        | Must be *HISTORICALREAD_RESPONSE*.                 |
+------------+--------------------+----------------------------------------------------+
|            | ClientHandler      | See :ref:`message_format`.                         |
+------------+--------------------+----------------------------------------------------+
| **Body**   |                    |                                                    |
+------------+--------------------+----------------------------------------------------+
|            | @DataValues        | The history data                                   |
+------------+--+-----------------+----------------------------------------------------+
|            |  | Value           |                                                    |
+------------+--+---+-------------+----------------------------------------------------+
|            |  |  | Body         | The value of the variable.                         |
+------------+--+---+-------------+----------------------------------------------------+
|            |  |  | Type         | The type of the variable.                          |
+------------+--+--+--------------+----------------------------------------------------+
|            |  | [Status]        | The OPC UA status of the variable                  |
|            |  |                 | if it is not *Success*.                            |
+------------+--+-----------------+----------------------------------------------------+
|            |  | SourceTimestamp | The time of the value given by the source in ISO   |
|            |  |                 | 8601 format. Example: "2015-09-06T09:03:21.237123" |
+------------+--+-----------------+----------------------------------------------------+
|            |  | ServerTimestamp | The time of the value given by the server in ISO   |
|            |  |                 | 8601 format. Example: "2015-09-06T09:03:21.237123" |
+------------+--+-----------------+----------------------------------------------------+

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
        'MessageType':'HISTORICALREAD_REQUEST',
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
                    #        "Type": "Boolean",
                    #        "Body": true
                    #    },
                    #    "SourceTimestamp": "2019-07-26T11:10:20Z",
                    #    "ServerTimestamp": "2019-07-26T11:10:20Z"
                    # }
                    #}


