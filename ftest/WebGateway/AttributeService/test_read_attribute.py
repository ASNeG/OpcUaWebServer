import json
from opcua import Client
from opcua import ua

from ..web_gateway_testcase import WebGatewayTestCase


class TestReadAttribute(WebGatewayTestCase):

    def setUp(self):
        WebGatewayTestCase.setUp(self)

        self.opcua_client = Client(self.OPC_SERVER_URL)
        self.opcua_client.connect()

    def tearDown(self):

        self.opcua_client.disconnect()

        WebGatewayTestCase.tearDown(self)

    def test_read(self):
        #
        # send read request to the opc ua server
        #
        node = self.opcua_client.get_node("ns=2;i=208")
        node.set_value(ua.DataValue(ua.Variant(555, ua.VariantType.Int32)))

        node = self.opcua_client.get_node("ns=2;i=220")
        node.set_value(ua.DataValue(True))

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
                            "Namespace": "2",
                            "Id": "208"
                        },
                        "AttributeId": "13"
                    },
                    {
                        "NodeId": {
                            "Namespace": "2",
                            "Id": "220"
                        },
                        "AttributeId": "13"
                    }
                ]
            }

        }
        
        print("SEND: ", json.dumps(req, indent = 4))
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
        self.assertEqual(len(res['Body']['Results']), 2)
        self.assertIsNotNone(res['Body']['Results'][0]['Value'])
        self.assertIsNotNone(res['Body']['Results'][1]['Value'])
        self.assertEqual('555', res['Body']['Results'][0]['Value']['Body'])
        self.assertEqual('true', res['Body']['Results'][1]['Value']['Body'])

    def test_big_size(self):
        NODE_COUNT = 1000
        nodesToRead = [
                {
                    "NodeId": {
                        "Namespace": "2",
                        "Id": "208"
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
