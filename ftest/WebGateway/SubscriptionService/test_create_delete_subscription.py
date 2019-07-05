import json

from ..web_gateway_testcase import WebGatewayTestCase


class TestSession(WebGatewayTestCase):

    def setUp(self):
        WebGatewayTestCase.setUp(self)

    def tearDown(self):
        WebGatewayTestCase.tearDown(self)

    def test_multiply_session(self):
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
        # send delete subscriptions request to the opc ua server
        #
        req = {
            "Header": {
                "MessageType": "GW_DeleteSubscriptionsRequest",
                "ClientHandle": "client-handle",
                "SessionId": self.sessionId
            },
            "Body": {
                "SubscriptionIds": [subscriptionId]
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
        self.assertEqual(res['Header']['MessageType'], "GW_DeleteSubscriptionsResponse")
        self.assertEqual(res['Header']['ClientHandle'], "client-handle")
        self.assertEqual(res['Header']['SessionId'], self.sessionId)
        self.assertEqual(res['Header']['StatusCode'], "0")
        self.assertEqual(res['Body']['Results'][0], "0")
