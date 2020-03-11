.. _attribute_services:

Attribute Services
==================

**WebSocket Gateway** provides a way to read and write attribute data through JSON API. To use this API you must initialize
an OPCUA session between the webgetway and the OPCUA server that you are going to communicate with. See :ref:`session` for more
details.

.. _gw_read_request:

Reading
-------
To read values of OPCUA attributes, you must form a JSON request in the following format:

GW_ReadRequest
``````````````
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
```````````````

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
```````````````````````

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


Writing
-------

To write values of OPCUA attributes, you must form a JSON request in the following format:

.. _gw_write_request:

GW_WriteRequest
`````````````````

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
`````````````````

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
+------------+---------------------+------------------------------------------------------------------+

Write Attributes Example
`````````````````````````

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
    json.loads(resp)    # => {
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


History Reading
---------------
.. _gw_history_read_request:

To read history values of OPCUA attributes, you must form a JSON request in the following format:

GW_HistoryReadRequest
``````````````````````
+----------------------------------+------------------------------------------------------------------+
| Field                            | Description                                                      |
+============+=====================+==================================================================+
| **Header** |                     |                                                                  |
+------------+---------------------+------------------------------------------------------------------+
|            | MessageType         | Must be *GW_HistoryReadRequest*.                                 |
+------------+---------------------+------------------------------------------------------------------+
|            | ClientHandler       | See :ref:`gw_message_format`.                                    |
+------------+---------------------+------------------------------------------------------------------+
| **Body**   |                     | The request has too many parameters to be described here.        |
|            |                     | See *Specification Part 4 Services, 5.10.3 History read* and     |
|            |                     | the example below.                                               |
+------------+---------------------+------------------------------------------------------------------+

.. _gw_history_read_response:

GW_HistoryReadResponse
``````````````````````
+----------------------------------+------------------------------------------------------------------+
| Field                            | Description                                                      |
+============+=====================+==================================================================+
| **Header** |                     |                                                                  |
+------------+---------------------+------------------------------------------------------------------+
|            | MessageType         | Must be *GW_HistoryReadResponse*.                                |
+------------+---------------------+------------------------------------------------------------------+
|            | ClientHandler       | See :ref:`gw_message_format`.                                    |
+------------+---------------------+------------------------------------------------------------------+
| **Body**   |                     | The request has too many parameters to be described here.        |
|            |                     | See *Specification Part 4 Services, 5.10.3 History read* and     |
|            |                     | the example below.                                               |
+------------+---------------------+------------------------------------------------------------------+

History Read Attributes Example
```````````````````````````````

This example works with the already opened session. See :ref:`gw_session_example`.

.. code-block:: python

    req = {
            "Header": {
                "MessageType": "GW_HistoryReadRequest",
                "ClientHandle": "client-handle",
                "SessionId": "1"
            },

            "Body": {
                "HistoryReadDetails": {
                    "TypeId": {"Id": "15263"},
                    "Body": {
                        "IsReadModified": "False",
                        "StartTime": "2019-05-04T14:33:04Z",
                        "EndTime": "2050-05-04T14:33:04Z",
                        "NumValuesPerNode": "0",
                        "ReturnBounds": "True"
                    }
                },
                "NodesToRead": [
                    {
                        "NodeId": {
                            "Namespace": "12",
                            "IdType": "1",
                            "Id": "DoubleValue"
                        }

                    }
                ]
            }
        }

    ws.send(json.dumps(req))
    resp = ws.recv()
    json.loads(resp)    # =>{
                        # "Header": {
                        #     "MessageType": "GW_HistoryReadResponse",
                        #     "ClientHandle": "client-handle",
                        #     "SessionId": "1",
                        #     "StatusCode": "0"
                        # },
                        # "Body": {
                        #     "Results": [
                        #         {
                        #             "StatusCode": "0",
                        #             "HistoryData": {
                        #                 "TypeId": {
                        #                     "Id": "15270"
                        #                 },
                        #                 "Body": {
                        #                     "DataValues": [
                        #                         {
                        #                             "Value": {
                        #                                 "Type": "11",
                        #                                 "Body": "0"
                        #                             },
                        #                             "SourceTimestamp": "2020-03-11T07:33:30Z"
                        #                         {
                        #                             "Value": {
                        #                                 "Type": "11",
                        #                                 "Body": "1"
                        #                             },
                        #                             "SourceTimestamp": "2020-03-11T07:33:29Z"
                        #                         },
                        #                         ...
                        #                     ]
                        #                 }
                        #             }
                        #         }
                        #     ]
                        #   }
                        # }


History Read Events Example
```````````````````````````````

This example works with the already opened session. See :ref:`gw_session_example`.

.. code-block:: python

    req = {
            "Header": {
                "MessageType": "GW_HistoryReadRequest",
                "ClientHandle": "client-handle",
                "SessionId": self.sessionId,
                "RequestTimeout": 10000,
            },
            "Body": {
                "HistoryReadDetails": {
                    "TypeId": {"Id": "15262"},
                    "Body": {
                        "StartTime": "2019-05-04T14:33:04Z",
                        "EndTime": "2050-05-04T14:33:04Z",
                        "NumValuesPerNode": "1000",
                        "Filter": {
                            "SelectClauses": [
                                {
                                    "TypeDefinitionId": {"Id": "2041"},
                                    "AttributeId": "13",
                                    "IndexRange": "",
                                    "BrowsePath": [{"Name": "EventId"}]
                                },
                                {
                                    "TypeDefinitionId": {"Id": "2041"},
                                    "AttributeId": "13",
                                    "IndexRange": "",
                                    "BrowsePath": [{"Name": "EventType"}]
                                },
                                {
                                    "TypeDefinitionId": {"Id": "2041"},
                                    "AttributeId": "13",
                                    "IndexRange": "",
                                    "BrowsePath": [{"Name": "SourceName"}]
                                },
                                {
                                    "TypeDefinitionId": {"Id": "2041"},
                                    "AttributeId": "13",
                                    "IndexRange": "",
                                    "BrowsePath": [{"Name": "Time"}]
                                },
                                {
                                    "TypeDefinitionId": {"Id": "2041"},
                                    "AttributeId": "13",
                                    "IndexRange": "",
                                    "BrowsePath": [{"Name": "Message"}]
                                },
                                {
                                    "TypeDefinitionId": {"Id": "2041"},
                                    "AttributeId": "13",
                                    "IndexRange": "",
                                    "BrowsePath": [{"Name": "Severity"}]
                                },
                                {
                                    "TypeDefinitionId": {"Id": "2041"},
                                    "AttributeId": "13",
                                    "IndexRange": "",
                                    "BrowsePath": [{"Name": "Prompt"}]
                                },
                                {
                                    "TypeDefinitionId": {"Id": "2041"},
                                    "AttributeId": "13",
                                    "IndexRange": "",
                                    "BrowsePath": [{"Name": "ResponseOptionSet"}]
                                },
                                {
                                    "TypeDefinitionId": {"Id": "2041"},
                                    "AttributeId": "13",
                                    "IndexRange": "",
                                    "BrowsePath": [{"Name": "Value"}]
                                },
                                {
                                    "TypeDefinitionId": {"Id": "2041"},
                                    "AttributeId": "13",
                                    "IndexRange": "",
                                    "BrowsePath": [{"Name": "Id"}]
                                },
                                {
                                    "TypeDefinitionId": {"Id": "2041"},
                                    "AttributeId": "13",
                                    "IndexRange": "",
                                    "BrowsePath": [{"Name": "AvtivateState"}]
                                },
                                {
                                    "TypeDefinitionId": {"Id": "2041"},
                                    "AttributeId": "13",
                                    "IndexRange": "",
                                    "BrowsePath": [{"Name": "ActivateState"}, {"Name": "Id"}]
                                }
                            ],
                            "WhereClause": {
                                "Elements": []
                            }
                        }
                    }
                },
                "NodesToRead": [
                    {
                        "NodeId": {
                            "Namespace": "12",
                            "IdType": "1",
                            "Id": "EventObject"
                        }

                    }
                ]
            }

        }

    ws.send(json.dumps(req))
    resp = ws.recv()
    json.loads(resp)        #=> {
                            #   "Header": {
                            #       "MessageType": "GW_HistoryReadResponse",
                            #       "ClientHandle": "client-handle",
                            #       "SessionId": "1",
                            #       "RequestTimeout": "10000",
                            #       "StatusCode": "0"
                            #   },
                            #   "Body": {
                            #       "Results": [
                            #           {
                            #               "StatusCode": "0",
                            #               "HistoryData": {
                            #                   "TypeId": {
                            #                       "Id": "15273"
                            #                   },
                            #                   "Body": {
                            #                       "Events": [
                            #                           {
                            #                               "EventFields": [
                            #                                   {
                            #                                       "Type": "15",
                            #                                       "Body": "yNulZM3G8gI="
                            #                                   },
                            #                                   {
                            #                                       "Type": "0"
                            #                                   },
                            #                                   {
                            #                                       "Type": "0"
                            #                                   },
                            #                                   {
                            #                                       "Type": "13",
                            #                                       "Body": "2020-03-11T08:18:19Z"
                            #                                   },
                            #                                   {
                            #                                       "Type": "21",
                            #                                       "Body": {
                            #                                           "Locale": "",
                            #                                           "Text": "EventObject"
                            #                                       }
                            #                                   },
                            #                                   {
                            #                                       "Type": "5",
                            #                                       "Body": "100"
                            #                                   },
                            #                                  ....
                            #                               ]
                            #                           },
                            #
                            #                           ...
                            #                       ]
                            #                   }
                            #               }
                            #           }
                            #       ]
                            #   }
                            # }