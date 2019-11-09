import json

from ..web_gateway_testcase import WebGatewayTestCase


class TestDataChangeNoitfy(WebGatewayTestCase):

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
                                "Namespace": "3",
                                "Id": "218"
                            }
                        },
                        "RequestedParameters": {
                            "ClientHandle": "4712",
                            "SamplingInterval": "1000"
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
        print("RECV: ", str)

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
        res = json.loads(str)

        WebGatewayTestCase.tearDown(self)

    def test_create_delete(self):

        #
        # receive data change request
        #
        for i in range(1, 5):
            str = self.ws.recv()
            print("RECV: ", str)
            res = json.loads(str)
            self.assertEqual(res['Header']['MessageType'], "GW_DataChangeNotify")
            self.assertEqual(res['Header']['ClientHandle'], "test_create_delete") 
            self.assertEqual(res['Header']['SessionId'], self.sessionId)
            self.assertEqual(res['Body']['ClientHandleData'], "4712")
            self.assertIsNotNone(res['Body']['Value'])
