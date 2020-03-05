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