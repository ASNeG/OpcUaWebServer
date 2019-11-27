.. _message_format:

Message Format
==============

All the JSON messages have the following structures:

+------------------------------+--------------------------------------------------------+
| Field                        | Description                                            | 
+============+=================+========================================================+
| **Header** |                 |                                                        |
+------------+-----------------+--------------------------------------------------------+
|            | MessageType     | The type of the JSON message. It can have              |
|            |                 | the following values:                                  |
|            |                 +--------------------------------------------------------+
|            |                 |     :ref:`READ_REQUEST<read_request>`                  |
|            |                 +--------------------------------------------------------+
|            |                 |     :ref:`READ_RESPONSE<read_response>`                |
|            |                 +--------------------------------------------------------+
|            |                 |     :ref:`WRITE_REQUEST<write_request>`                |
|            |                 +--------------------------------------------------------+
|            |                 |     :ref:`WRITE_RESPONSE<write_response>`              |
|            |                 +--------------------------------------------------------+
|            |                 |     :ref:`VALUELIST_REQUEST<valuelist_request>`        |
|            |                 +--------------------------------------------------------+
|            |                 |     :ref:`VALUELIST_RESPONSE<valuelist_response>`      |
|            |                 +--------------------------------------------------------+
|            |                 |     :ref:`VALUEINFO_REQUEST<valueinfo_request>`        |
|            |                 +--------------------------------------------------------+
|            |                 |     :ref:`VALUEINFO_RESPONSE<valueinfo_response>`      |
|            |                 +--------------------------------------------------------+
|            |                 |     :ref:`MONITORSTART_REQUEST<monitorstart_request>`  |  
|            |                 +--------------------------------------------------------+
|            |                 |     :ref:`MONITORSTART_RESPONSE<monitorstart_response>`|
|            |                 +--------------------------------------------------------+
|            |                 |     :ref:`MONITORSTOP_REQUEST<monitorstop_request>`    |
|            |                 +--------------------------------------------------------+
|            |                 |     :ref:`MONITORSTOP_RESPONSE<monitorstop_response>`  |
|            |                 +--------------------------------------------------------+
|            |                 |     :ref:`MONITORUPDATE_MESSAGE<monitorupdate_message>`|
|            |                 +--------------------------------------------------------+
|            |                 |     :ref:`HISTORICALREAD_REQUEST<hisread_request>`     |
|            |                 +--------------------------------------------------------+
|            |                 |     :ref:`HISTORICALREAD_RESPONSE<hisread_response>`   |
+------------+-----------------+--------------------------------------------------------+
|            | ClientHandler   | An identifier of the message which is set by           |
|            |                 | the client in the request. The server copies its       |
|            |                 | value to the corresponding response.                   |
+------------+-----------------+--------------------------------------------------------+
|            | [StatusCode]    | Is sent if an error occurs by processing the           |
|            |                 | request from the client.                               |
+------------+-----------------+--------------------------------------------------------+
| **Body**   |                 |                                                        |
+------------+-----------------+--------------------------------------------------------+

All the JSON messages are described by using our :ref:`notation`.
