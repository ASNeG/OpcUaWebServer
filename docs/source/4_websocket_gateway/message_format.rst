.. _gw_message_format:

Message Format
--------------

All the HTTP requests from a client to the gateway must have a :term:`JSON` message in the following format:

+------------------------------+------------------------------------------------------------+
| Field                        | Description                                                |
+============+=================+============================================================+
| **Header** |                 |                                                            |
+------------+-----------------+------------------------------------------------------------+
|            | MessageType     | The type of the JSON message. It can have                  |
|            |                 | the following values:                                      |
|            |                 +------------------------------------------------------------+
|            |                 | :ref:`GW_LoginRequest<gw_login_request>`                   |
|            |                 +------------------------------------------------------------+
|            |                 | :ref:`GW_LoginResponse<gw_login_response>`                 |
|            |                 +------------------------------------------------------------+
|            |                 | :ref:`GW_SessionStatusNotify<gw_session_status_notify>`    |
|            |                 +------------------------------------------------------------+
|            |                 | :ref:`GW_LogoutRequest<gw_logout_request>`                 |
|            |                 +------------------------------------------------------------+
|            |                 | :ref:`GW_LogoutResponse<gw_logout_response>`               |
|            |                 +------------------------------------------------------------+
|            |                 | :ref:`GW_ReadRequest<gw_read_request>`                     |
|            |                 +------------------------------------------------------------+
|            |                 | :ref:`GW_ReadResponse<gw_read_response>`                   |
|            |                 +------------------------------------------------------------+
|            |                 | :ref:`GW_WriteRequest<gw_write_request>`                   |
|            |                 +------------------------------------------------------------+
|            |                 | :ref:`GW_WriteResponse<gw_write_response>`                 |
|            |                 +------------------------------------------------------------+
|            |                 | :ref:`GW_HistoryReadRequest<gw_history_read_request>`      |
|            |                 +------------------------------------------------------------+
|            |                 | :ref:`GW_HistoryReadResponse<gw_history_read_response>`    |
|            |                 +------------------------------------------------------------+
|            |                 | GW_CreateSubscriptionRequest                               |
|            |                 +------------------------------------------------------------+
|            |                 | GW_CreateSubscriptionResponse                              |
|            |                 +------------------------------------------------------------+
|            |                 | GW_DeleteSubscriptionRequest                               |
|            |                 +------------------------------------------------------------+
|            |                 | GW_DeleteSubscriptionResponse                              |
|            |                 +------------------------------------------------------------+
|            |                 | GW_CreateMonitoredItemsRequest                             |
|            |                 +------------------------------------------------------------+
|            |                 | GW_CreateMonitoredItemsResponse                            |
|            |                 +------------------------------------------------------------+
|            |                 | GW_DeleteMonitoredItemsRequest                             |
|            |                 +------------------------------------------------------------+
|            |                 | GW_DeleteMonitoredItemsResponse                            |
|            |                 +------------------------------------------------------------+
|            |                 | GW_DataChangeNotify                                        |
|            |                 +------------------------------------------------------------+
|            |                 | GW_EventNotify                                             |
|------------+-----------------+------------------------------------------------------------+
|            | ClientHandler   | An identifier of the message which is set by               |
|            |                 | the client in the request. The server copies its           |
|            |                 | value to the corresponding response.                       |
+------------+-----------------+------------------------------------------------------------+
|            | [StatusCode]    | OPC UA Status Code of the response. The gateway copies the |
|            |                 | status code that the OPC UA server sends to the gateway in |
|            |                 | the response on the OPCUA request. If the gateway fails    |
|            |                 | to build the OPCUA request (e.g. malformed JSON request)   |
|            |                 | then the status is *BadInvalidArgument*                    |
+------------+-----------------+------------------------------------------------------------+
|            | [SessionId]     | The integer identifier of session. It have to be in        |
|            |                 |                                                            |
+------------+-----------------+------------------------------------------------------------+
| **Body**   |                 |  MessageType specific                                      |
+------------+-----------------+------------------------------------------------------------+

All the JSON messages are described by using our :ref:`notation`.

Common OPCUA Structures
-----------------------

OPCUA has some common data structures that have the same format in the different services. So we describe them
here to avoid of the duplication in other sections of the document. You can find more information about. how
OPCUA types are encoded in JSON, in *SPEC 6 "Mapping" Section 5.4.2*.

.. _node_id_json:

NodeId
``````
An unique identifier of OPC UA Node

+------------+-----------------+------------------------------------------------------------+
| **NodId**  |                 |                                                            |
+------------+-----------------+------------------------------------------------------------+
|            | [Namespace]     | Number of namepsace (0,1,2..). Default 0.                  |
+------------+-----------------+------------------------------------------------------------+
|            | Id              | Identifier can be Number, String, GUID or BinaryString     |
+------------+-----------------+------------------------------------------------------------+
|            | [TypeId]        |  The IdentifierType encoded as a JSON number:              |
|            |                 |  Allowed values are:                                       |
|            |                 |    0 - UInt32 Identifier                                   |
|            |                 |    1 - A String Identifier                                 |
|            |                 |    2 - A Guid Identifier encoded                           |
|            |                 |    3 - A ByteString Identifier                             |
+------------+-----------------+------------------------------------------------------------+

.. _variant_json:

Variant
```````
A type that can contain the value of different types.

+------------+-----------------+------------------------------------------------------------+
| **Variant**|                 |                                                            |
+------------+-----------------+------------------------------------------------------------+
|            | Type            | The OPCUA code of the value type                           |
+------------+-----------------+------------------------------------------------------------+
|            | Body            | The value                                                  |
+------------+-----------------+------------------------------------------------------------+
|            | [Dimensions]    | The dimensions of the array encoded                        |
|            |                 |  as an JSON array of JSON numbers. Default scalar value.   |
+------------+-----------------+------------------------------------------------------------+

.. _data_value_json:

DataValue
`````````

+---------------+--------------------+------------------------------------------------------------------+
| **DataValue** |                    |                                                                  |
+---------------+--------------------+------------------------------------------------------------------+
|               | Value              | See :ref:`variant_json`                                          |
+---------------+--------------------+------------------------------------------------------------------+
|               | [Status]           | The OPC UA status of the variable if it is not *Success*         |
+---------------+--------------------+------------------------------------------------------------------+
|               | [SourceTimestamp]  | The time of the value given by the source in ISO                 |
|               |                    | 8601 format. Example: "2015-09-06T09:03:21Z"                     |
+---------------+--------------------+------------------------------------------------------------------+
|               | [SourcePicoSeconds]| The number of 10 picosecond intervals for the SourceTimestamp    |
+---------------+--------------------+------------------------------------------------------------------+
|               | [ServerTimestamp]  | The time of the value given by the server in ISO                 |
|               |                    | 8601 format. Example: "2015-09-06T09:03:21Z"                     |
+---------------+--------------------+------------------------------------------------------------------+
|               | [ServerPicoSeconds]| The number of 10 picosecond intervals for the ServerTimestamp    |
+---------------+--------------------+------------------------------------------------------------------+
