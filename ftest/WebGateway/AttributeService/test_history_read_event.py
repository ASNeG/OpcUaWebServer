import json

from ..web_gateway_testcase import WebGatewayTestCase


class TestHistoryReadEvent(WebGatewayTestCase):

    def setUp(self):
        WebGatewayTestCase.setUp(self)

    def tearDown(self):
        WebGatewayTestCase.tearDown(self)
        
    def test_history_read(self):
        #
        # send history read event request to the opc ua server
        #
        req = {
            "Header": {
                "MessageType": "GW_HistoryReadRequest",
                "ClientHandle": "client-handle",
                "SessionId": self.sessionId
            },
            "Body": {
                "HistoryReadDetails": {
                    "TypeId": {"Id": "15262"},
                    "Body": {
                        "StartTime": "2019-05-04T14:33:04Z",
                        "EndTime": "2050-05-04T14:33:04Z",
                        "NumValuesPerNode": "1000",
                        "Filter": {
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
                },
                "NodesToRead": [
                    {
                        "NodeId": {
                            "Namespace": "12",
                            "IdType": "1",
                            "Id": "EventObject"
                        }

                    }
                ]
            }

        }
        
        print("SEND: ", json.dumps(req, indent=4))
        self.ws.send(json.dumps(req)) 
        
        
        #
        # receive history read response from the opc ua server
        #
        str = self.ws.recv()
        print("RECV: ", str)
        res = json.loads(str)
        self.assertEqual(res['Header']['MessageType'], "GW_HistoryReadResponse")
        self.assertEqual(res['Header']['ClientHandle'], "client-handle")
        self.assertEqual(res['Header']['SessionId'], self.sessionId)
        self.assertEqual(res['Header']['StatusCode'], "0")
        self.assertEqual(len(res['Body']['Results']), 1)
        self.assertEqual(len(res['Body']['Results'][0]['HistoryData']['Body']['Events']), 5)
        self.assertEqual(res['Body']['Results'][0]['StatusCode'], "0")