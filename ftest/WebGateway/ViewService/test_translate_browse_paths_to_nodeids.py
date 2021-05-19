import json
from opcua import Client
from opcua import ua

from ..web_gateway_testcase import WebGatewayTestCase


class TestTranslateBrowsePathsToNodeIds(WebGatewayTestCase):

    def setUp(self):
        WebGatewayTestCase.setUp(self)

        self.opcua_client = Client(self.OPC_SERVER_URL)
        self.opcua_client.connect()

    def tearDown(self):

        self.opcua_client.disconnect()

        WebGatewayTestCase.tearDown(self)

    def test_translate_browse_paths_to_nodeids_request(self):
        #
        # send translate browse paths to node ids request to the opc ua server
        #
        req = {
            "Header": {
                "MessageType": "GW_TranslateBrowsePathsToNodeIdsRequest",
                "ClientHandle": "client-handle",
                "SessionId": self.sessionId
            },
            "Body": {
                "BrowsePaths": [
                    {
                        "StartingNode": {
                            "Id": "85"
                        },
                        "RelativePath" : { 
                            "Elements" : [
                                {
                                    "IsInverse" : "false",
                                    "IncludeSubtypes" : "false",
                                    "TargetName" : {
                                        "Name": "Server"
                                    }
                                },
                                {
                                    "IsInverse" : "false",
                                    "IncludeSubtypes" : "false",
                                    "TargetName" : {
                                        "Name": "ServerStatus"
                                    }
                                }
                            ]
                        }
                    }
                ]
            }

        }
        
        print("SEND: ", json.dumps(req, indent = 4))
        self.ws.send(json.dumps(req)) 
        
        
        #
        # receive translate browse paths to node ids response from the opc ua server
        #
        str = self.ws.recv()
        print("RECV: ", str)
        res = json.loads(str)
        self.assertEqual(res['Header']['MessageType'], "GW_TranslateBrowsePathsToNodeIdsResponse")
        self.assertEqual(res['Header']['ClientHandle'], "client-handle")
        self.assertEqual(res['Header']['SessionId'], self.sessionId)
        self.assertEqual(res['Header']['StatusCode'], "0")
        #self.assertEqual(len(res['Body']['Results']), 2)
        #self.assertIsNotNone(res['Body']['Results'][0]['Value'])
        #self.assertIsNotNone(res['Body']['Results'][1]['Value'])
        #self.assertEqual('555', res['Body']['Results'][0]['Value']['Body'])
        #self.assertEqual('true', res['Body']['Results'][1]['Value']['Body'])
