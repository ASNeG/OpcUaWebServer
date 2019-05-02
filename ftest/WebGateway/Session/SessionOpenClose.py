#!/usr/bin/python3

from websocket import create_connection
import json

# open web socket connection
ws = create_connection("ws://127.0.0.1:8082")

# send login request to open opc ua session
req = {
    "Header" : {
        "MessageType" : "GW_LoginRequest",
        "ClientHandle" : "client-handle"
     },
     "Body" : {
         "DiscoveryUrl" : "opc.tcp://127.0.0.1:8889"
     }
}
print("SEND: ", req)
ws.send(json.dumps(req)) 

result = ws. recv()
print("RECV: ", result)


# close opc ua session

# close web socket connection
#ws.close()