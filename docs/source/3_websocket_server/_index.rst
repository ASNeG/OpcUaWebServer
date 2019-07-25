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
|            | MessageType     | Type of the JSON message. It can have              |
|            |                 | the following values:                              |
|            |                 +----------------------------------------------------+
|            |                 |     READ_REQUEST                                   |
|            |                 +----------------------------------------------------+
|            |                 |     READ_RESPONSE                                  |
|            |                 +----------------------------------------------------+
|            |                 |     WRITE_REQUEST                                  |
|            |                 +----------------------------------------------------+
|            |                 |     WRITE_RESPONSE                                 |
|            |                 +----------------------------------------------------+
|            |                 |     VALUELIST_REQUEST                              |
|            |                 +----------------------------------------------------+
|            |                 |     VALUELIST_RESPONSE                             |
|            |                 +----------------------------------------------------+
|            |                 |     VALUEINFO_REQUEST                              |
|            |                 +----------------------------------------------------+
|            |                 |     VALUEINFO_RESPONSE                             |
|            |                 +----------------------------------------------------+
|            |                 |     MONITORSTART_REQUEST                           |  
|            |                 +----------------------------------------------------+
|            |                 |     MONITORSTART_RESPONSE                          |
|            |                 +----------------------------------------------------+
|            |                 |     MONITORSTOP_REQUEST                            |
|            |                 +----------------------------------------------------+
|            |                 |     MONITORSTOP_RESPONSE                           |
|            |                 +----------------------------------------------------+
|            |                 |     MONITORUPDATE_MESSAGE                          |
|            |                 +----------------------------------------------------+
|            |                 |     HISTORICALREAD_REQUEST                         |
|            |                 +----------------------------------------------------+
|            |                 |     HISTORICALREAD_RESPONSE                        |
+------------+-----------------+----------------------------------------------------+
|            | ClientHandler   | An identifier of the message which is set by       |
|            |                 | the client in the request. The server copy its     |
|            |                 | value to the corresponding response.               |
+------------+-----------------+----------------------------------------------------+
|            | [StatusCode]    | Is sent if an error occurs by processing the       |
|            |                 | request from the client.                           |
+------------+-----------------+----------------------------------------------------+
| **Body**   |                 |                                                    |
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

**Status Codes**

+-----------------------+-----------------------------------------------------------+
| Status Code           | Description                                               |
+-----------------------+-----------------------------------------------------------+
| BadInternalError      | The server failed to process the request due to internal  |
|                       | error.                                                    |
+-----------------------+-----------------------------------------------------------+


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

Knowing the variable names the client can get the information about the configuration of the variables
by using *ValueInfo* request.

**Request**

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

**Status Codes**

+-----------------------+-----------------------------------------------------------+
| Status Code           | Description                                               |
+-----------------------+-----------------------------------------------------------+
| BadInternalError      | The server failed to process the request due to internal  |
|                       | error.                                                    |
+-----------------------+-----------------------------------------------------------+
| BadAttributeInvalid   | There is no variable names in the request.                | 
+-----------------------+-----------------------------------------------------------+
| BadNodeIdUnknown      | The variable name isn't found in the server configuration.| 
+-----------------------+-----------------------------------------------------------+

**Example in Python**

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


Read
````

HistoricalRead
```````````````

Write
`````

MonitorSartResponse
```````````````````

MonitorStopResponse
```````````````````

