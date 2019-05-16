#!/usr/bin/python3

from websocket import create_connection
import json
import Check as c

#
# open web socket connection
#
ws = create_connection("ws://127.0.0.1:8082")

#
# send first login request to open opc ua session
#
req = {
    "Header" : {
        "MessageType" : "GW_LoginRequest",
        "ClientHandle" : "client-handle"
     },
     "Body" : {
         "DiscoveryUrl" : "opc.tcp://127.0.0.1:8889"
     }
}
print("SEND: ", json.dumps(req, indent = 4))
ws.send(json.dumps(req)) 

str = ws. recv()
print("RECV: ", str)
res = json.loads(str)
c.checkEqual(res['Header']['MessageType'], "GW_LoginResponse")
c.checkEqual(res['Header']['ClientHandle'], "client-handle")
c.checkEqual(res['Header']['StatusCode'], "Success")
c.checkExists(res['Body']['SessionId'])
sessionId1 = res['Body']['SessionId']


#
# receive session status notify for first session
#
str = ws. recv()
print("RECV: ", str)
res = json.loads(str)
c.checkEqual(res['Header']['MessageType'], "GW_SessionStatusNotify")
c.checkEqual(res['Header']['ClientHandle'], "client-handle")
c.checkEqual(res['Header']['SessionId'], sessionId1)
c.checkEqual(res['Body']['SessionStatus'], "Connect")


#
# send second login request to open opc ua session
#
req = {
    "Header" : {
        "MessageType" : "GW_LoginRequest",
        "ClientHandle" : "client-handle"
     },
     "Body" : {
         "DiscoveryUrl" : "opc.tcp://127.0.0.1:8889"
     }
}
print("SEND: ", json.dumps(req, indent = 4))
ws.send(json.dumps(req)) 

str = ws. recv()
print("RECV: ", str)
res = json.loads(str)
c.checkEqual(res['Header']['MessageType'], "GW_LoginResponse")
c.checkEqual(res['Header']['ClientHandle'], "client-handle")
c.checkEqual(res['Header']['StatusCode'], "Success")
c.checkExists(res['Body']['SessionId'])
sessionId2 = res['Body']['SessionId']


#
# receive session status notify for second session
#
str = ws. recv()
print("RECV: ", str)
res = json.loads(str)
c.checkEqual(res['Header']['MessageType'], "GW_SessionStatusNotify")
c.checkEqual(res['Header']['ClientHandle'], "client-handle")
c.checkEqual(res['Header']['SessionId'], sessionId2)
c.checkEqual(res['Body']['SessionStatus'], "Connect")



#
# close web socket connection
#
ws.close()
