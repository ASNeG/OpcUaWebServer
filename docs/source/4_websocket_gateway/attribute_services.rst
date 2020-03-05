.. _attribute_services:

Attribute Services
==================

**WebSocket Gateway** provides a way to read and write attribute data through JSON API. To use this API you must initialize
an OPCUA session between the webgetway and the OPCUA server that you are going to communicate with. See :ref:`session` for more
details.

.. _gw_read_request:

GW_ReadRequest
---------------

To read a value of an OPCUA attribute, you must form a JSON request in the following format:

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
|            | @Results            | A list of the values                                             |
+------------+---+-----------------+------------------------------------------------------------------+
|            |   | Value           | See :ref:`variant_json`                                          |
+------------+---+-----------------+------------------------------------------------------------------+
|            |   | [Status]        | The OPC UA status of the variable if it is not *Success*         |
+------------+---+-----------------+------------------------------------------------------------------+
|            |   | SourceTimestamp | The time of the value given by the source in ISO                 |
|            |   |                 | 8601 format. Example: "2015-09-06T09:03:21Z"                     |
+------------+---+-----------------+------------------------------------------------------------------+
|            |   |ServerTimestamp  | The time of the value given by the server in ISO                 |
|            |   |                 | 8601 format. Example: "2015-09-06T09:03:21Z"                     |
+------------+---+-----------------+------------------------------------------------------------------+


Read Attribute Example
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


.. _gw_write_response:

GW_WriteResponse
----------------


.. _gw_history_read_request:

GW_HistoryReadRequest
----------------------


.. _gw_history_read_response:

GW_HistoryReadResponse
----------------------

