import json

from ..web_gateway_testcase import WebGatewayTestCase


class TestWrite(WebGatewayTestCase):

    def setUp(self):
        WebGatewayTestCase.setUp(self)

    def tearDown(self):
        WebGatewayTestCase.tearDown(self)

    def test_write(self):
        #
        # send write request to the opc ua server
        #
        req = {
            "Header": {
                "MessageType": "GW_WriteRequest",
                "ClientHandle": "client-handle",
                "SessionId": self.sessionId
            },
            "Body": {
                "NodesToWrite": [
                    {
                        "NodeId": {
                            "Namespace": "3",
                            "Id": "218"
                        },
                        "Value": {
                            "Value": {
                                "Type": "11",
                                "Body": "80"
                            }
                        }
                    },
                    {
                        "NodeId": {
                            "Namespace": "3",
                            "Id": "219"
                        },
                        "Value": {
                            "Value": {
                                "Type": "11",
                                "Body": [
                                    "2.10921e+06",
                                    "2.10921e+06",
                                    "2.10922e+06"
                                ]
                            }
                        }
                    }
                ]
            }

        }
        
        print("SEND: ", json.dumps(req, indent = 4))
        self.ws.send(json.dumps(req)) 
        
        
        #
        # receive write response from the opc ua server
        #
        str = self.ws. recv()
        print("RECV: ", str)
        res = json.loads(str)
        self.assertEqual(res['Header']['MessageType'], "GW_WriteResponse")
        self.assertEqual(res['Header']['ClientHandle'], "client-handle")
        self.assertEqual(res['Header']['SessionId'], self.sessionId)
        self.assertEqual(res['Header']['StatusCode'], "0")
        self.assertEqual(len(res['Body']['Results']), 2)
        self.assertEqual(res['Body']['Results'][0], "0")
        self.assertEqual(res['Body']['Results'][1], "0")
