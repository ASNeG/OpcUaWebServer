import json

from ..web_gateway_testcase import WebGatewayTestCase


class TestEventNotify(WebGatewayTestCase):

    def setUp(self):
        WebGatewayTestCase.setUp(self)

        #
        # send create subscription request to the opc ua server
        #
        req = {
            "Header": {
                "MessageType": "GW_CreateSubscriptionRequest",
                "ClientHandle": "client-handle",
                "SessionId": self.sessionId
            },
            "Body": {
            }

        }
        print("SEND: ", json.dumps(req, indent=4))
        self.ws.send(json.dumps(req))

        #
        # receive create subscription response from the opc ua server
        #
        str = self.ws.recv()
        print("RECV: ", str)
        res = json.loads(str)
        self.subscriptionId = res['Body']['SubscriptionId']

        #
        # send create monitored items request
        #
        req = {
            "Header": {
                "MessageType": "GW_CreateMonitoredItemsRequest",
                "ClientHandle": "client-handle",
                "SessionId": self.sessionId
            },
            "Body": {
                "SubscriptionId": self.subscriptionId,
                "ItemsToCreate": [
                    {
                        "ItemToMonitor": {
                            "NodeId": {
                                "Namespace": "14",
                                "IdType": "1",
                                "Id": "Event11"
                            }
                        },
                        "RequestedParameters": {
                            "ClientHandle": "4713",
                            "SamplingInterval": "500",
                            "Filter": {
                                "TypeId": {"Id": "15295"},
                                "Body": {
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
                        }
                    }
                ]
            }

        }
        print("SEND: ", json.dumps(req, indent=4))
        self.ws.send(json.dumps(req))

        #
        # receive monitored create items response
        #
        str = self.ws.recv()
        print("RECV: ", str)
        res = json.loads(str)
        self.monitoredItemId = res['Body']['Results'][0]['MonitoredItemId']

    def tearDown(self):
        #
        # send delete monitored items request
        #
        req = {
            "Header": {
                "MessageType": "GW_DeleteMonitoredItemsRequest",
                "ClientHandle": "client-handle",
                "SessionId": self.sessionId
            },
            "Body": {
                "SubscriptionId": self.subscriptionId,
                "MonitoredItemIds": [self.monitoredItemId]
            }

        }
        print("SEND: ", json.dumps(req, indent=4))
        self.ws.send(json.dumps(req))

        #
        # receive delete monitored items response
        #
        str = self.ws.recv()

        #
        # send delete subscriptions request to the opc ua server
        #
        req = {
            "Header": {
                "MessageType": "GW_DeleteSubscriptionsRequest",
                "ClientHandle": "client-handle",
                "SessionId": self.sessionId
            },
            "Body": {
                "SubscriptionIds": [self.subscriptionId]
            }

        }
        print("SEND: ", json.dumps(req, indent=4))
        self.ws.send(json.dumps(req))

        #
        # receive delete subscriptions response from the opc ua server
        #
        str = self.ws.recv()
        print("RECV: ", str)

        WebGatewayTestCase.tearDown(self)

    def test_event_notify(self):
        #
        # receive event notify
        #
        for i in range(1, 5):
            str = self.ws.recv()
            print("RECV: ", str)
            res = json.loads(str)
            self.assertEqual(res['Header']['MessageType'], "GW_EventNotify")
            self.assertEqual(res['Header']['ClientHandle'], "test_event_notify") 
            self.assertEqual(res['Header']['SessionId'], self.sessionId)
            self.assertEqual(res['Body']['ClientHandleData'], "4713")
