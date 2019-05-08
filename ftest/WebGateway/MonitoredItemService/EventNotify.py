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
print("SEND: ", json.dumps(req, indent = 4))
ws.send(json.dumps(req)) 

#
# receive login response deom opc ua server
#
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
# send create subscription request to the opc ua server
#
req = {
    "Header" : {
        "MessageType" : "GW_CreateSubscriptionRequest",
        "ClientHandle" : "client-handle",
        "SessionId" : sessionId
    },
    "Body" : {
    }

}
print("SEND: ", json.dumps(req, indent = 4))
ws.send(json.dumps(req)) 


#
# receive create subscription response from the opc ua server
#
str = ws. recv()
print("RECV: ", str)
res = json.loads(str)
c.checkEqual(res['Header']['MessageType'], "GW_CreateSubscriptionResponse")
c.checkEqual(res['Header']['ClientHandle'], "client-handle")
c.checkEqual(res['Header']['SessionId'], sessionId)
c.checkEqual(res['Header']['StatusCode'], "Success")
c.checkExists(res['Body']['SubscriptionId'])
c.checkExists(res['Body']['RevisedPublishingInterval'])
c.checkExists(res['Body']['RevisedLifetimeCount'])
c.checkExists(res['Body']['RevisedMaxKeepAliveCount'])
subscriptionId = res['Body']['SubscriptionId']


#
# send create monitored items request
#
req = {
    "Header" : {
        "MessageType" : "GW_CreateMonitoredItemsRequest",
        "ClientHandle" : "client-handle",
        "SessionId" : sessionId
    },
    "Body" : {
	"SubscriptionId" : subscriptionId,
        "ItemsToCreate" : [
            {
                "ItemToMonitor" : {
                    "NodeId" : {
                        "Namespace" : "14",
                        "IdType" : "1", 
                        "Id" : "Event11"
                    }
                },
                "RequestedParameters" : {
                    "ClientHandle" : "4713",
                    "SamplingInterval" : "500",
		    "Filter" : {
                        "TypeId" : { "Id" : "15295"} ,
	                "Body" : {
                            "SelectClauses" : [
                                {
                                    "TypeDefinitionId" : { "Id" : "2041" },
                                    "AttributeId" : "13",
                                    "IndexRange" : "",
                                    "BrowsePath" : [ { "Name" : "EventId" } ]
                                },
                                {
                                    "TypeDefinitionId" : { "Id" : "2041" },
                                    "AttributeId" : "13",
                                    "IndexRange" : "",
                                    "BrowsePath" : [ { "Name" : "EventType" } ]
                                },
                                {
                                    "TypeDefinitionId" : { "Id" : "2041" },
                                    "AttributeId" : "13",
                                    "IndexRange" : "",
                                    "BrowsePath" : [ { "Name" : "SourceName" } ]
                                },
                                {
                                    "TypeDefinitionId" : { "Id" : "2041" },
                                    "AttributeId" : "13",
                                    "IndexRange" : "",
                                    "BrowsePath" : [ { "Name" : "Time" } ]
                                },
                                {
                                    "TypeDefinitionId" : { "Id" : "2041" },
                                    "AttributeId" : "13",
                                    "IndexRange" : "",
                                    "BrowsePath" : [ { "Name" : "Message" } ]
                                },
                                {
                                    "TypeDefinitionId" : { "Id" : "2041" },
                                    "AttributeId" : "13",
                                    "IndexRange" : "",
                                    "BrowsePath" : [ { "Name" : "Severity" } ]
                                }
                                #{
                                #    "TypeDefinitionId" : { "Id" : "2041" },
                                #    "AttributeId" : "13",
                                #    "IndexRange" : "",
                                #    "BrowsePath" : [ { "Name" : "Prompt" } ]
                                #}
                                #{
                                #    "TypeDefinitionId" : { "Id" : "2041" },
                                #    "AttributeId" : "13",
                                #    "IndexRange" : "",
                                #    "BrowsePath" : [ { "Name" : "ResponseOptionSet" } ]
                                #}
                            ],
                            "WhereClause" : {
                                "Elements" : []
                            }
       	                }
                    }
                }
            }
	]
    }

}
print("SEND: ", json.dumps(req, indent = 4))
ws.send(json.dumps(req)) 


#
# receive monitored create items response
#
str = ws. recv()
print("RECV: ", str)
res = json.loads(str)
c.checkEqual(res['Header']['MessageType'], "GW_CreateMonitoredItemsResponse")
c.checkEqual(res['Header']['ClientHandle'], "client-handle")
c.checkEqual(res['Header']['SessionId'], sessionId)
c.checkEqual(res['Header']['StatusCode'], "Success")
c.checkEqual(res['Body']['Results'][0]['StatusCode'], "Success")
c.checkExists(res['Body']['Results'][0]['MonitoredItemId'])
monitoredItemId = res['Body']['Results'][0]['MonitoredItemId']


#
# receive event notify
#
for i in range(1, 5):
    str = ws. recv()
    print("RECV: ", str)
    res = json.loads(str)
    c.checkEqual(res['Header']['MessageType'], "GW_EventNotify")
    c.checkEqual(res['Header']['ClientHandle'], "client-handle")
    c.checkEqual(res['Header']['SessionId'], sessionId)
    c.checkEqual(res['Body']['ClientHandleData'], "4713")


#
# send delete monitored items request
#
req = {
    "Header" : {
        "MessageType" : "GW_DeleteMonitoredItemsRequest",
        "ClientHandle" : "client-handle",
        "SessionId" : sessionId
    },
    "Body" : {
	"SubscriptionId" : subscriptionId,
        "MonitoredItemIds" : [ monitoredItemId ]
    }

}
print("SEND: ", json.dumps(req, indent = 4))
ws.send(json.dumps(req)) 


#
# receive delete monitored items response
#
str = ws. recv()
print("RECV: ", str)
res = json.loads(str)
c.checkEqual(res['Header']['MessageType'], "GW_DeleteMonitoredItemsResponse")
c.checkEqual(res['Header']['ClientHandle'], "client-handle")
c.checkEqual(res['Header']['SessionId'], sessionId)
c.checkEqual(res['Header']['StatusCode'], "Success")
c.checkEqual(res['Body']['Results'][0], "Success")


#
# send delete subscriptions request to the opc ua server
#
req = {
    "Header" : {
        "MessageType" : "GW_DeleteSubscriptionsRequest",
        "ClientHandle" : "client-handle",
        "SessionId" : sessionId
    },
    "Body" : {
        "SubscriptionIds" : [ subscriptionId ]
    }

}
print("SEND: ", json.dumps(req, indent = 4))
ws.send(json.dumps(req)) 


#
# receive delete subscriptions response from the opc ua server
#
str = ws. recv()
print("RECV: ", str)
res = json.loads(str)
c.checkEqual(res['Header']['MessageType'], "GW_DeleteSubscriptionsResponse")
c.checkEqual(res['Header']['ClientHandle'], "client-handle")
c.checkEqual(res['Header']['SessionId'], sessionId)
c.checkEqual(res['Header']['StatusCode'], "Success")
c.checkEqual(res['Body']['Results'][0], "Success")


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
print("SEND: ", json.dumps(req, indent = 4))
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
