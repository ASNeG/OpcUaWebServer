import json
from opcua import Client
from opcua import ua

from ..web_gateway_testcase import WebGatewayTestCase


class TestBrowse(WebGatewayTestCase):

    def setUp(self):
        WebGatewayTestCase.setUp(self)

        self.opcua_client = Client(self.OPC_SERVER_URL)
        self.opcua_client.connect()

    def tearDown(self):

        self.opcua_client.disconnect()

        WebGatewayTestCase.tearDown(self)

    def test_browse_request(self):
        #
        # send browse request to the opc ua server
        #
        req = {
            "Header": {
                "MessageType": "GW_BrowseRequest",
                "ClientHandle": "client-handle",
                "SessionId": self.sessionId
            },
            "Body": {
                "RequestedMaxReferencesPerNode" : "100",
                "NodesToBrowse": [
                    {
                        "NodeId": {
                            "Namespace" : "0",
                            "Id": "85"
                        },
                        "ReferenceTypeId" : {
                            "Namespace" : "0",
                            "Id" : "33"
                        },
                        "BrowseDirection" : "0",
                        "IncludeSubtypes" : "TRUE",
                        "NodeClassMask" : "16",
                        "ResultMask" : "63"
                    }
                ]
            }

        }
        
        print("SEND: ", json.dumps(req, indent = 4))
        self.ws.send(json.dumps(req)) 
        
        
        #
        # receive browse response from the opc ua server
        #
        str = self.ws.recv()
        print("RECV: ", str)
        res = json.loads(str)
        self.assertEqual(res['Header']['MessageType'], "GW_BrowseResponse")
        self.assertEqual(res['Header']['ClientHandle'], "client-handle")
        self.assertEqual(res['Header']['SessionId'], self.sessionId)
        self.assertEqual(res['Header']['StatusCode'], "0")
        #self.assertEqual(len(res['Body']['Results']), 2)
        #self.assertIsNotNone(res['Body']['Results'][0]['Value'])
        #self.assertIsNotNone(res['Body']['Results'][1]['Value'])
        #self.assertEqual('555', res['Body']['Results'][0]['Value']['Body'])
        #self.assertEqual('true', res['Body']['Results'][1]['Value']['Body'])
