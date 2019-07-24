.. _websocket_server:

WebSocket Server
================

TODO

JSON API
--------

TODO

ValueList
`````````

With *ValueList* request the client can get the list of OPC UA :term:`Variable`\ s described in :ref:`opcuaclient_config`.

**Request**

+------------------------------+----------------------------------------------------+
| Field                        | Description                                        | 
+============+=================+====================================================+
| **Header** |                 |                                                    |
+------------+-----------------+----------------------------------------------------+
|            | MessageType     | Should be *VALUELIST_REQUEST*                      |
+------------+-----------------+----------------------------------------------------+
|            | ClientHandler   | See :ref:`header_format`                           |
+------------+-----------------+----------------------------------------------------+
| **Body**   |                 | Empty                                              |
+------------+-----------------+----------------------------------------------------+

**Response**

+------------------------------+----------------------------------------------------+
| Field                        | Description                                        | 
+============+=================+====================================================+
| **Header** |                 |                                                    |
+------------+-----------------+----------------------------------------------------+
|            | MessageType     | Should be *VALUELIST_RESPONSE*                     |
+------------+-----------------+----------------------------------------------------+
|            | ClientHandler   | See :ref:`header_format`                           |
+------------+-----------------+----------------------------------------------------+
| **Body**   |                 |                                                    |
+------------+-----------------+----------------------------------------------------+
|            | Variables       | List of the variable names                         |
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

