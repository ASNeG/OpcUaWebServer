#!/usr/bin/python3

from websocket import create_connection
import json
import Check as c

#
# open web socket connection
#
ws = create_connection("ws://127.0.0.1:8082")

#
# send login request to open opc ua session
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
print("SEND: ", req)
ws.send(json.dumps(req)) 

str = ws. recv()
print("RECV: ", str)
res = json.loads(str)
c.checkEqual(res['Header']['MessageType'], "GW_LoginResponse")
c.checkEqual(res['Header']['ClientHandle'], "client-handle")
c.checkEqual(res['Header']['StatusCode'], "Success")
c.checkExists(res['Body']['SessionId'])
sessionId = res['Body']['SessionId']


#
# receive session status notify
#
str = ws. recv()
print("RECV: ", str)
res = json.loads(str)
c.checkEqual(res['Header']['MessageType'], "GW_SessionStatusNotify")
c.checkEqual(res['Header']['ClientHandle'], "client-handle")
c.checkEqual(res['Header']['SessionId'], sessionId)
c.checkEqual(res['Body']['SessionStatus'], "Connect")


#
# send read request to the opc ua server
#
req = {
    "Header" : {
        "MessageType" : "GW_ReadRequest",
        "ClientHandle" : "client-handle",
        "SessionId" : sessionId
    },
    "Body" : {
        "NodesToRead" : [
            {
                "NodeId" : {
		    "Id" : "4711"
                }
            }
	]         
    }

}
print("SEND: ", req)
ws.send(json.dumps(req)) 


#
# receive read response from the opc ua server
#
str = ws. recv()
print("RECV: ", str)
res = json.loads(str)
c.checkEqual(res['Header']['MessageType'], "GW_ReadResponse")
c.checkEqual(res['Header']['ClientHandle'], "client-handle")
c.checkEqual(res['Header']['SessionId'], sessionId)
c.checkEqual(res['Header']['StatusCode'], "Success")


#
# send logout request to close opc ua session
#
req = {
    "Header" : {
        "MessageType" : "GW_LogoutRequest",
        "ClientHandle" : "client-handle",
        "SessionId" : sessionId
    },
    "Body" : {
    }

}
print("SEND: ", req)
ws.send(json.dumps(req)) 

str = ws. recv()
print("RECV: ", str)
res = json.loads(str)
c.checkEqual(res['Header']['MessageType'], "GW_LogoutResponse")
c.checkEqual(res['Header']['ClientHandle'], "client-handle")
c.checkEqual(res['Header']['SessionId'], sessionId)
c.checkEqual(res['Header']['StatusCode'], "Success")




#
# close web socket connection
#
ws.close()
