import json

from ..web_gateway_testcase import WebGatewayTestCase


class TestCall(WebGatewayTestCase):

    def setUp(self):
        WebGatewayTestCase.setUp(self)

    def tearDown(self):
        WebGatewayTestCase.tearDown(self)
    
    def test_call(self):
        #
        # send call request to the opc ua server
        #
        req = {
            "Header": {
                "MessageType": "GW_CallRequest",
                "ClientHandle": "client-handle",
                "SessionId": self.sessionId
            },
            "Body": {
                "MethodsToCall": [
                    {
                        "ObjectId": {
                            "Namespace": "6",
                            "IdType": "1",
                            "Id": "Function"
                        },
                        "MethodId": {
                            "Namespace": "6",
                            "IdType": "1",
                            "Id": "func1"
                        }
                    }
                ]
            }

        }
        print("SEND: ", json.dumps(req, indent = 4))
        self.ws.send(json.dumps(req)) 
        
        
        #
        # receive history read response from the opc ua server
        #
        str = self.ws. recv()
        print("RECV: ", str)
        res = json.loads(str)
        self.assertEqual(res['Header']['MessageType'], "GW_CallResponse")
        self.assertEqual(res['Header']['ClientHandle'], "client-handle")
        self.assertEqual(res['Header']['SessionId'], self.sessionId)
        self.assertEqual(res['Header']['StatusCode'], "0")
        self.assertEqual(len(res['Body']['Results']), 1)
        self.assertEqual(res['Body']['Results'][0]['StatusCode'], "0")

    def test_call_in(self):
        #
        # send call request to the opc ua server
        #
        req = {
            "Header": {
                "MessageType": "GW_CallRequest",
                "ClientHandle": "client-handle",
                "SessionId": self.sessionId
            },
            "Body": {
                "MethodsToCall": [
                    {
                        "ObjectId": {
                            "Namespace": "6",
                            "IdType": "1",
                            "Id": "Function"
                        },
                        "MethodId": {
                            "Namespace": "6",
                            "IdType": "1",
                            "Id": "func2"
                        },
                        "InputArguments": [
                            {
                                "Type": "7",
                                "Body": "11"
                            },
                            {
                                "Type": "11",
                                "Body": "12"
                            }
                        ]
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
        self.assertEqual(res['Header']['MessageType'], "GW_CallResponse")
        self.assertEqual(res['Header']['ClientHandle'], "client-handle")
        self.assertEqual(res['Header']['SessionId'], self.sessionId)
        self.assertEqual(res['Header']['StatusCode'], "0")
        self.assertEqual(len(res['Body']['Results']), 1)
        self.assertEqual(res['Body']['Results'][0]['StatusCode'], "0")

    def test_call_in_out(self):
        #
        # send call request to the opc ua server
        #
        req = {
            "Header": {
                "MessageType": "GW_CallRequest",
                "ClientHandle": "client-handle",
                "SessionId": self.sessionId
            },
            "Body": {
                "MethodsToCall": [
                    {
                        "ObjectId": {
                            "Namespace": "6",
                            "IdType": "1",
                            "Id": "Function"
                        },
                        "MethodId": {
                            "Namespace": "6",
                            "IdType": "1",
                            "Id": "funcMult"
                        },
                        "InputArguments": [
                            {
                                "Type": "7",
                                "Body": "5"
                            },
                            {
                                "Type": "7",
                                "Body": "6"
                            }
                        ]
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
        self.assertEqual(res['Header']['MessageType'], "GW_CallResponse")
        self.assertEqual(res['Header']['ClientHandle'], "client-handle")
        self.assertEqual(res['Header']['SessionId'], self.sessionId)
        self.assertEqual(res['Header']['StatusCode'], "0")
        self.assertEqual(len(res['Body']['Results']), 1)
        self.assertEqual(res['Body']['Results'][0]['StatusCode'], "0")
        self.assertEqual(len(res['Body']['Results'][0]['OutputArguments']), 1)
        self.assertEqual(res['Body']['Results'][0]['OutputArguments'][0]['Type'], "7")
        self.assertEqual(res['Body']['Results'][0]['OutputArguments'][0]['Body'], "30")

    def call_out(self):
        #
        # send call request to the opc ua server
        #
        req = {
            "Header": {
                "MessageType": "GW_CallRequest",
                "ClientHandle": "client-handle",
                "SessionId": self.sessionId
            },
            "Body": {
                "MethodsToCall": [
                    {
                        "ObjectId": {
                            "Namespace": "6",
                            "IdType": "1",
                            "Id": "Function"
                        },
                        "MethodId": {
                            "Namespace": "6",
                            "IdType": "1",
                            "Id": "func3"
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
        self.assertEqual(res['Header']['MessageType'], "GW_CallResponse")
        self.assertEqual(res['Header']['ClientHandle'], "client-handle")
        self.assertEqual(res['Header']['SessionId'], self.sessionId)
        self.assertEqual(res['Header']['StatusCode'], "0")
        self.assertEqual(len(res['Body']['Results']), 1)
        self.assertEqual(res['Body']['Results'][0]['StatusCode'], "0")
        self.assertEqual(len(res['Body']['Results'][0]['OutputArguments']), 2)
