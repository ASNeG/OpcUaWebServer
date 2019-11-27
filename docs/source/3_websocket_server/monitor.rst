.. _monitoring:

Monitoring
==========

**OpcUaWebServer** provides a subscription model. The client can subscribe to a variable
by using *MONITORSTART_REQUEST*. After that the server sends the data of the variable as *MONITORUPDATE_MESSAGE*
only when it changes. Finally, the client must stop monitoring the value
and send *MONITORSTOP_REQUEST* request.

.. _monitorstart_request:

Monitor Start Request
---------------------

+------------------------------+----------------------------------------------------+
| Field                        | Description                                        | 
+============+=================+====================================================+
| **Header** |                 |                                                    |
+------------+-----------------+----------------------------------------------------+
|            | MessageType     | Must be *MONITORSTART_REQUEST*.                    |
+------------+-----------------+----------------------------------------------------+
|            | ClientHandler   | See :ref:`message_format`.                         |
+------------+-----------------+----------------------------------------------------+
| **Body**   |                 |                                                    |
+------------+-----------------+----------------------------------------------------+
|            | Variable        | The variable to read.                              |
+------------+-----------------+----------------------------------------------------+

.. _monitorstart_response:

Monitor Start Response
----------------------

+------------------------------+----------------------------------------------------+
| Field                        | Description                                        | 
+============+=================+====================================================+
| **Header** |                 |                                                    |
+------------+-----------------+----------------------------------------------------+
|            | MessageType     | Must be *MONITORSTART_RESPONSE*.                   |
+------------+-----------------+----------------------------------------------------+
|            | ClientHandler   | See :ref:`message_format`.                         |
+------------+-----------------+----------------------------------------------------+
| **Body**   |                 |                                                    |
+------------+-----------------+----------------------------------------------------+
|            | [Status]        | The OPC UA status if it is not *Success*.          |
+------------+-----------------+----------------------------------------------------+

Monitor Start Status Codes
--------------------------

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

.. _monitorupdate_message:

Monitor Update Message
----------------------

+------------------------------+----------------------------------------------------+
| Field                        | Description                                        | 
+============+=================+====================================================+
| **Header** |                 |                                                    |
+------------+-----------------+----------------------------------------------------+
|            | MessageType     | Must be *MONITORUPDATE_MESSAGE*.                   |
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

.. _monitorstop_request:

Monitor Stop Request
---------------------

+------------------------------+----------------------------------------------------+
| Field                        | Description                                        | 
+============+=================+====================================================+
| **Header** |                 |                                                    |
+------------+-----------------+----------------------------------------------------+
|            | MessageType     | Must be *MONITORSTOP_REQUEST*.                     |
+------------+-----------------+----------------------------------------------------+
|            | ClientHandler   | See :ref:`message_format`.                         |
+------------+-----------------+----------------------------------------------------+
| **Body**   |                 |                                                    |
+------------+-----------------+----------------------------------------------------+
|            | Variable        | The variable to read.                              |
+------------+-----------------+----------------------------------------------------+

.. _monitorstop_response:

Monitor Stop Response
----------------------

+------------------------------+----------------------------------------------------+
| Field                        | Description                                        | 
+============+=================+====================================================+
| **Header** |                 |                                                    |
+------------+-----------------+----------------------------------------------------+
|            | MessageType     | Must be *MONITORSTOP_RESPONSE*.                    |
+------------+-----------------+----------------------------------------------------+
|            | ClientHandler   | See :ref:`message_format`.                         |
+------------+-----------------+----------------------------------------------------+
| **Body**   |                 |                                                    |
+------------+-----------------+----------------------------------------------------+
|            | [Status]        | The OPC UA status if it is not *Success*.          |
+------------+-----------------+----------------------------------------------------+


Monitor Stop Status Codes
--------------------------

+-----------------------+-----------------------------------------------------------+
| Status Code           | Description                                               |
+=======================+===========================================================+
| BadInternalError      | The server failed to process the request due to internal  |
|                       | error.                                                    |
+-----------------------+-----------------------------------------------------------+
| BadAttributeInvalid   | The server failed decode the body of the message.         | 
+-----------------------+-----------------------------------------------------------+
| BadNoEntryExists      | The variable name isn't found in the server configuration.| 
+-----------------------+-----------------------------------------------------------+


Example in Python
-----------------

.. code-block:: python

  import websocket
  import json

  msg = {
      'Header': {
        'MessageType':'MONITORSTART_REQUEST',
        'ClientHandle':'1'
      },
      'Body': { 'Variable' : 'Boolean'}
   }

  ws = websocket.create_connection('ws://127.0.0.1:8081')
  ws.send(json.dumps(msg)) 
  resp = ws.recv() 
  json.loads(resp)  #=> {
                    # 'Header': {
                    #   'MessageType': 'MONITORSTART_RESPONSE', 
                    #   'ClientHandle': '1'}, 
                    #  'Body': ''
                    # }
  
  resp = ws.recv() 
  json.loads(resp)  #=> {
                    # "Header": {
                    #    "MessageType": "MONITORUPDATE_MESSAGE",
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
  msg = {
      'Header': {
        'MessageType':'MONITORSTOP_REQUEST',
        'ClientHandle':'1'
      },
      'Body': { 'Variable' : 'Boolean'}
  }
  
  ws.send(json.dumps(msg)) 
  resp = ws.recv() 
  json.loads(resp)  #=> {
                    # 'Header': {
                    #   'MessageType': 'MONITORSTOP_RESPONSE', 
                    #   'ClientHandle': '1'}, 
                    #  'Body': ''
                    # }

