.. _attribute_services:

Attribute Services
==================

**WebSocket Gateway** provides a way to read and write attribute data through JSON API. To use this API you must initialize
an OPCUA session between the webgetway and the OPCUA server that you are going to communicate with. See :ref:`session` for more
details.

.. _gw_read_request:

GW_ReadRequest
---------------

To read values of OPCUA attributes, you must form a JSON request in the following format:

+----------------------------------+------------------------------------------------------------------+
| Field                            | Description                                                      |
+============+=====================+==================================================================+
| **Header** |                     |                                                                  |
+------------+---------------------+------------------------------------------------------------------+
|            | MessageType         | Must be *GW_ReadRequest*.                                        |
+------------+---------------------+------------------------------------------------------------------+
|            | ClientHandler       | See :ref:`gw_message_format`.                                    |
+------------+---------------------+------------------------------------------------------------------+
| **Body**   |                     |                                                                  |
+------------+---------------------+------------------------------------------------------------------+
|            | @NodesToRead        | A list of nodes to read                                          |
+------------+---+-----------------+------------------------------------------------------------------+
|            |   | NodeId          | See :ref:`node_id_json`                                          |
+------------+---+-----------------+------------------------------------------------------------------+
|            |   | [AttributeId]   | ID of the attribute to read. Defualt is 13 (Value)               |
+------------+---+-----------------+------------------------------------------------------------------+

.. _gw_read_response:

GW_ReadResponse
---------------

+----------------------------------+------------------------------------------------------------------+
| Field                            | Description                                                      |
+============+=====================+==================================================================+
| **Header** |                     |                                                                  |
+------------+---------------------+------------------------------------------------------------------+
|            | MessageType         | Must be *GW_ReadResponse*.                                       |
+------------+---------------------+------------------------------------------------------------------+
|            | ClientHandler       | See :ref:`gw_message_format`.                                    |
+------------+---------------------+------------------------------------------------------------------+
| **Body**   |                     |                                                                  |
+------------+---------------------+------------------------------------------------------------------+
|            | @Results            | A list of the :ref:`data_value_json`\ s                          |
+------------+---+-----------------+------------------------------------------------------------------+


Read Attributes Example
----------------------

This example works with the already opened session. See :ref:`gw_session_example`.

.. code-block:: python

    req = {
            "Header": {
                "MessageType": "GW_ReadRequest",
                "ClientHandle": "client-handle",
                "SessionId": "1"
            },
            "Body": {
                "NodesToRead": [
                    {
                        "NodeId": {
                            "Namespace": "2",
                            "Id": "208"
                        },
                        "AttributeId": "13"
                    },
                    {
                        "NodeId": {
                            "Namespace": "2",
                            "Id": "220"
                        },
                        "AttributeId": "13"
                    }
                ]
            }

        }

    ws.send(json.dumps(req))
    resp = ws.recv()
    json.loads(resp) => # {
                        # "Header": {
                        #   "MessageType": "GW_ReadResponse",
                        #   "ClientHandle": "client-handle",
                        #   "SessionId": "1",
                        #   "StatusCode": "0"
                        # },
                        # "Body": {
                        #   "Results": [
                        #       {
                        #           "Value": {
                        #               "Type": "6",
                        #               "Body": "123"
                        #           },
                        #           "SourceTimestamp": "2020-03-05T07:44:17Z",
                        #           "ServerTimestamp": "2020-03-05T07:44:17Z"
                        #       },
                        #       {
                        #           "Value": {
                        #               "Type": "1",
                        #               "Body": "true"
                        #            },
                        #           "SourceTimestamp": "2020-03-05T07:44:17Z",
                        #           "ServerTimestamp": "2020-03-05T07:44:17Z"
                        #       }
                        #   ]
                        # }
                        #}


.. _gw_write_request:

GW_WriteRequest
---------------

To write values of OPCUA attributes, you must form a JSON request in the following format:

+----------------------------------+------------------------------------------------------------------+
| Field                            | Description                                                      |
+============+=====================+==================================================================+
| **Header** |                     |                                                                  |
+------------+---------------------+------------------------------------------------------------------+
|            | MessageType         | Must be *GW_WriteRequest*.                                       |
+------------+---------------------+------------------------------------------------------------------+
|            | ClientHandler       | See :ref:`gw_message_format`.                                    |
+------------+---------------------+------------------------------------------------------------------+
| **Body**   |                     |                                                                  |
+------------+---------------------+------------------------------------------------------------------+
|            | @NodesToWrite       | A list of nodes to write                                         |
+------------+---+-----------------+------------------------------------------------------------------+
|            |   | NodeId          | See :ref:`node_id_json`                                          |
+------------+---+-----------------+------------------------------------------------------------------+
|            |   | [AttributeId]   | ID of the attribute to read. Defualt is 13 (Value)               |
+------------+---+-----------------+------------------------------------------------------------------+
|            |   | Value           | The value to write :ref:`data_value_json`                        |
+------------+---+-----------------+------------------------------------------------------------------+

.. _gw_write_response:

GW_WriteResponse
----------------

+----------------------------------+------------------------------------------------------------------+
| Field                            | Description                                                      |
+============+=====================+==================================================================+
| **Header** |                     |                                                                  |
+------------+---------------------+------------------------------------------------------------------+
|            | MessageType         | Must be *GW_WriteResponse*.                                      |
+------------+---------------------+------------------------------------------------------------------+
|            | ClientHandler       | See :ref:`gw_message_format`.                                    |
+------------+---------------------+------------------------------------------------------------------+
| **Body**   |                     |                                                                  |
+------------+---------------------+------------------------------------------------------------------+
|            | @Results            | A list of nodes StatusCodes                                      |
+------------+----- ---------------+------------------------------------------------------------------+

Write Attributes Example
----------------------

This example works with the already opened session. See :ref:`gw_session_example`.

.. code-block:: python

    req = {
       "Header": {
           "MessageType": "GW_WriteRequest",
           "ClientHandle": "client-handle",
           "SessionId": "1"
       },
       "Body": {
           "NodesToWrite": [
               {
                   "NodeId": {
                       "Namespace": "3",
                       "Id": "218"
                   },
                   "Value": {
                       "Value": {
                           "Type": "11",
                           "Body": "80"
                       }
                   }
               },
               {
                   "NodeId": {
                       "Namespace": "3",
                       "Id": "219"
                   },
                   "Value": {
                       "Value": {
                           "Type": "11",
                           "Body": [
                               "2.10921e+06",
                               "2.10921e+06",
                               "2.10922e+06"
                           ]
                       }
                   }
               }
           ]
       }
    }

    ws.send(json.dumps(req))
    resp = ws.recv()
    json.loads(resp) => # {
                        #   "Header": {
                        #       "MessageType": "GW_WriteResponse",
                        #       "ClientHandle": "client-handle",
                        #       "SessionId": "1",
                        #       "StatusCode": "0"
                        #   },
                        #   "Body": {
                        #       "Results": [
                        #           "0",
                        #           "0"
                        #       ]
                        #   }
                        #}



.. _gw_history_read_request:

GW_HistoryReadRequest
----------------------


.. _gw_history_read_response:

GW_HistoryReadResponse
----------------------

