.. _valuelist:

Value List
==========

With *VALUELIST_REQUEST* request the client can get the list of OPC UA :term:`Variable`\ s 
described in :ref:`opcuaclient_config`.

.. _valuelist_request:

Value List Request
------------------

+------------------------------+----------------------------------------------------+
| Field                        | Description                                        | 
+============+=================+====================================================+
| **Header** |                 |                                                    |
+------------+-----------------+----------------------------------------------------+
|            | MessageType     | Must be *VALUELIST_REQUEST*.                       |
+------------+-----------------+----------------------------------------------------+
|            | ClientHandler   | See :ref:`message_format`.                         |
+------------+-----------------+----------------------------------------------------+
| **Body**   |                 | Empty.                                             |
+------------+-----------------+----------------------------------------------------+

.. _valuelist_response:

Value List Response
-------------------

+------------------------------+----------------------------------------------------+
| Field                        | Description                                        | 
+============+=================+====================================================+
| **Header** |                 |                                                    |
+------------+-----------------+----------------------------------------------------+
|            | MessageType     | Must be *VALUELIST_RESPONSE*.                      |
+------------+-----------------+----------------------------------------------------+
|            | ClientHandler   | See :ref:`message_format`.                         |
+------------+-----------------+----------------------------------------------------+
| **Body**   |                 |                                                    |
+------------+-----------------+----------------------------------------------------+
|            | @Variables      | The list of the variable names.                    |
+------------+-----------------+----------------------------------------------------+

Status Codes
------------

+-----------------------+-----------------------------------------------------------+
| Status Code           | Description                                               |
+=======================+===========================================================+
| BadInternalError      | The server failed to process the request due to internal  |
|                       | error.                                                    |
+-----------------------+-----------------------------------------------------------+


Example in Python
-----------------

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


