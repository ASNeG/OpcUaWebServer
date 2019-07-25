.. _websocket_server:

WebSocket Server
================

TODO Introduce and notation

Message Format
``````````````
All the JSON messages have the following structures:

+------------------------------+----------------------------------------------------+
| Field                        | Description                                        | 
+============+=================+====================================================+
| **Header** |                 |                                                    |
+------------+-----------------+----------------------------------------------------+
|            | MessageType     | Type of the JSON message. It can be:               |

+------------+-----------------+----------------------------------------------------+
|            | ClientHandler   | An identifier of the message which is set by       |
|            |                 | the client in the request. The server copy its     |
|            |                 | value to the corresponding response.               |
+------------+-----------------+----------------------------------------------------+
| **Body**   |                 | Empty                                              |
+------------+-----------------+----------------------------------------------------+
|            | [StatusCode]    | Is sent if an error occurs by processing the       |
|            |                 | request from the client.                           |
+------------+-----------------+----------------------------------------------------+



JSON API
--------

.. _message_format:


ValueList
`````````

With *ValueList* request the client can get the list of OPC UA :term:`Variable`\ s described in :ref:`opcuaclient_config`.

**Request**

+------------------------------+----------------------------------------------------+
| Field                        | Description                                        | 
+============+=================+====================================================+
| **Header** |                 |                                                    |
+------------+-----------------+----------------------------------------------------+
|            | MessageType     | Should be *VALUELIST_REQUEST*.                     |
+------------+-----------------+----------------------------------------------------+
|            | ClientHandler   | See :ref:`message_format`.                         |
+------------+-----------------+----------------------------------------------------+
| **Body**   |                 | Empty.                                             |
+------------+-----------------+----------------------------------------------------+

**Response**

+------------------------------+----------------------------------------------------+
| Field                        | Description                                        | 
+============+=================+====================================================+
| **Header** |                 |                                                    |
+------------+-----------------+----------------------------------------------------+
|            | MessageType     | Should be *VALUELIST_RESPONSE*.                    |
+------------+-----------------+----------------------------------------------------+
|            | ClientHandler   | See :ref:`message_format`.                         |
+------------+-----------------+----------------------------------------------------+
| **Body**   |                 |                                                    |
+------------+-----------------+----------------------------------------------------+
|            | @Variables      | List of the variable names.                        |
+------------+-----------------+----------------------------------------------------+


**Example in Python**

.. code-block:: python

  import websocket
  import json

  msg = {
    'Header': {
      'MessageType':'VALUELIST_REQUEST',
      'ClientHandle':'1'
    },
    'Body': {}
  }
 
  ws = websocket.create_connection('ws://127.0.0.1:8081')
  ws.send(json.dumps(msg)) 
  resp = ws.recv()  
  json.loads(resp)  #=> { 
                    # 'Header': {
                    #   'ClientHandle': '1', 
                    #   'MessageType': 'VALUELIST_RESPONSE'
                    # },
                    # 
                    # 'Body': {
                    #   'Variables': [
                    #     'Var1',
                    #     'Var2', .. 
                    #   ] 
                    # }
                    #}

  


ValueInfo
`````````

Read
````

HistoricalRead
``````````````

Write
`````

MonitorSartResponse
```````````````````

MonitorStopResponse
```````````````````

