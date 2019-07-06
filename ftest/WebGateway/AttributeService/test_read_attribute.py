import json
import unittest

from ..web_gateway_testcase import WebGatewayTestCase


class TestReadAttribute(WebGatewayTestCase):

    def setUp(self):
        WebGatewayTestCase.setUp(self)

    def tearDown(self):
        WebGatewayTestCase.tearDown(self)

    def test_read(self):
        #
        # send read request to the opc ua server
        #
        req = {
            "Header": {
                "MessageType": "GW_ReadRequest",
                "ClientHandle": "client-handle",
                "SessionId": self.sessionId
            },
            "Body": {
                "NodesToRead": [
                    {
                        "NodeId": {
                            "Namespace": "3",
                            "Id": "218"
                        },
                        "AttributeId": "14"
                    },
                    {
                        "NodeId": {
                            "Namespace": "3",
                            "Id": "219"
                        },
                        "AttributeId": "14"
                    }
                ]
            }

        }
        
        print("SEND: ", json.dumps(req, indent = 4))
        self.ws.send(json.dumps(req)) 
        
        
        #
        # receive read response from the opc ua server
        #
        str = self.ws. recv()
        print("RECV: ", str)
        res = json.loads(str)
        self.assertEqual(res['Header']['MessageType'], "GW_ReadResponse")
        self.assertEqual(res['Header']['ClientHandle'], "client-handle")
        self.assertEqual(res['Header']['SessionId'], self.sessionId)
        self.assertEqual(res['Header']['StatusCode'], "0")
        self.assertEqual(len(res['Body']['Results']), 2)
        self.assertIsNotNone(res['Body']['Results'][0]['Value'])
        self.assertIsNotNone(res['Body']['Results'][1]['Value'])

    @unittest.skip("what is the node count?")
    def test_big_size(self):
        NODE_COUNT = 2000
        nodesToRead = [
                {
                    "NodeId": {
                        "Namespace": "3",
                        "Id": "218"
                    }
                }
                for _ in range(0, NODE_COUNT)
        ]

        req = {
            "Header": {
                "MessageType": "GW_ReadRequest",
                "ClientHandle": "client-handle",
                "SessionId": self.sessionId
            },
            "Body": {
                "NodesToRead": nodesToRead
            }

        }
        print("SEND: ", json.dumps(req, indent=4))
        self.ws.send(json.dumps(req))

        #
        # receive read response from the opc ua server
        #
        str = self.ws.recv()
        print("RECV: ", str)
        res = json.loads(str)
        self.assertEqual(res['Header']['MessageType'], "GW_ReadResponse")
        self.assertEqual(res['Header']['ClientHandle'], "client-handle")
        self.assertEqual(res['Header']['SessionId'], self.sessionId)
        self.assertEqual(res['Header']['StatusCode'], "0")
        self.assertEqual(len(res['Body']['Results']), NODE_COUNT)
        self.assertIsNotNone(res['Body']['Results'][0]['Value'])
        self.assertIsNotNone(res['Body']['Results'][1]['Value'])