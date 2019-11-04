import json

from ..web_gateway_testcase import WebGatewayTestCase


class TestMonitoredItem(WebGatewayTestCase):

    def setUp(self):
        WebGatewayTestCase.setUp(self)

    def tearDown(self):
        WebGatewayTestCase.tearDown(self)

    def test_create_delete(self):
        #
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
        self.assertEqual(res['Header']['MessageType'], "GW_CreateSubscriptionResponse")
        self.assertEqual(res['Header']['ClientHandle'], "client-handle")
        self.assertEqual(res['Header']['SessionId'], self.sessionId)
        self.assertEqual(res['Header']['StatusCode'], "0")
        self.assertIsNotNone(res['Body']['SubscriptionId'])
        self.assertIsNotNone(res['Body']['RevisedPublishingInterval'])
        self.assertIsNotNone(res['Body']['RevisedLifetimeCount'])
        self.assertIsNotNone(res['Body']['RevisedMaxKeepAliveCount'])
        subscriptionId = res['Body']['SubscriptionId']


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
                "SubscriptionId": subscriptionId,
                "ItemsToCreate": [
                    {
                        "ItemToMonitor": {
                            "NodeId": {
                                "Id": "2258"
                            }
                        },
                        "RequestedParameters": {
                            "ClientHandle": "4711",
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
        self.assertEqual(res['Header']['MessageType'], "GW_CreateMonitoredItemsResponse")
        self.assertEqual(res['Header']['ClientHandle'], "client-handle")
        self.assertEqual(res['Header']['SessionId'], self.sessionId)
        self.assertEqual(res['Header']['StatusCode'], "0")
        self.assertEqual(res['Body']['Results'][0]['StatusCode'], "0")
        self.assertIsNotNone(res['Body']['Results'][0]['MonitoredItemId'])
        monitoredItemId = res['Body']['Results'][0]['MonitoredItemId']


        #
        # receive data change request
        #
        str = self.ws.recv()
        print("RECV: ", str)
        res = json.loads(str)
        self.assertEqual(res['Header']['MessageType'], "GW_DataChangeNotify")
        self.assertEqual(res['Header']['ClientHandle'], "test_create_delete")
        self.assertEqual(res['Header']['SessionId'], self.sessionId)
        self.assertEqual(res['Body']['ClientHandleData'], "4711")
        self.assertIsNotNone(res['Body']['Value'])


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
                "SubscriptionId": subscriptionId,
                "MonitoredItemIds": [monitoredItemId]
            }

        }
        print("SEND: ", json.dumps(req, indent=4))
        self.ws.send(json.dumps(req))


        #
        # receive delete monitored items response
        #
        str = self.ws.recv()
        print("RECV: ", str)
        res = json.loads(str)
        self.assertEqual(res['Header']['MessageType'], "GW_DeleteMonitoredItemsResponse")
        self.assertEqual(res['Header']['ClientHandle'], "client-handle")
        self.assertEqual(res['Header']['SessionId'], self.sessionId)
        self.assertEqual(res['Header']['StatusCode'], "0")
        self.assertEqual(res['Body']['Results'][0], "0")


        #
        # send delete subscriptions request to the opc ua server
        #
        req = {
            "Header" : {
                "MessageType" : "GW_DeleteSubscriptionsRequest",
                "ClientHandle" : "client-handle",
                "SessionId" : self.sessionId
            },
            "Body" : {
                "SubscriptionIds" : [ subscriptionId ]
            }

        }
        print("SEND: ", json.dumps(req, indent = 4))
        self.ws.send(json.dumps(req))


        #
        # receive delete subscriptions response from the opc ua server
        #
        str = self.ws.recv()
        print("RECV: ", str)
        res = json.loads(str)
        self.assertEqual(res['Header']['MessageType'], "GW_DeleteSubscriptionsResponse")
        self.assertEqual(res['Header']['ClientHandle'], "client-handle")
        self.assertEqual(res['Header']['SessionId'], self.sessionId)
        self.assertEqual(res['Header']['StatusCode'], "0")
        self.assertEqual(res['Body']['Results'][0], "0")
