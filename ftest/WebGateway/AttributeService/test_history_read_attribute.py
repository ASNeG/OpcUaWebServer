import json

from ..web_gateway_testcase import WebGatewayTestCase


class TestHistoryReadAttribute(WebGatewayTestCase):

    def setUp(self):
        WebGatewayTestCase.setUp(self)

    def tearDown(self):
        WebGatewayTestCase.tearDown(self)

    def test_history_read(self):
        #
        # send history read request to the opc ua server
        #
        req = {
            "Header": {
                "MessageType": "GW_HistoryReadRequest",
                "ClientHandle": "client-handle",
                "SessionId": self.sessionId
            },

            "Body": {
                "HistoryReadDetails": {
                    "TypeId": {"Id": "15263"},
                    "Body": {
                        "IsReadModified": "False",
                        "StartTime": "2019-05-04T14:33:04Z",
                        "EndTime": "2050-05-04T14:33:04Z",
                        "NumValuesPerNode": "0",
                        "ReturnBounds": "True"
                    }
                },
                "NodesToRead": [
                    {
                        "NodeId": {
                            "Namespace": "12",
                            "IdType": "1",
                            "Id": "DoubleValue"
                        }

                    }
                ]
            }

        }
        print("SEND: ", json.dumps(req, indent=4))
        self.ws.send(json.dumps(req))

        #
        # receive history read attribute response from the opc ua server
        #
        str = self.ws.recv()
        print("RECV: ", str)
        res = json.loads(str)
        self.assertEqual(res['Header']['MessageType'], "GW_HistoryReadResponse")
        self.assertEqual(res['Header']['ClientHandle'], "client-handle")
        self.assertEqual(res['Header']['SessionId'], self.sessionId)
        self.assertEqual(res['Header']['StatusCode'], "0")
        self.assertEqual(len(res['Body']['Results']), 1)
        self.assertEqual(len(res['Body']['Results'][0]['HistoryData']['Body']['DataValues']), 10)
        self.assertEqual(res['Body']['Results'][0]['StatusCode'], "0")
