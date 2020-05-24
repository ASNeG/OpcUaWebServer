import json
import unittest

from ..web_gateway_testcase import WebGatewayTestCase


class TestSession(WebGatewayTestCase):

    def setUp(self):
        WebGatewayTestCase.setUp(self)

    def tearDown(self):
        WebGatewayTestCase.tearDown(self)

    def test_multiply_session(self):
        #
        # send second login request to open opc ua session
        #
        req = {
            "Header": {
                "MessageType": "GW_LoginRequest",
                "ClientHandle": "client-handle"
            },
            "Body": {
                "DiscoveryUrl":  self.OPC_SERVER_URL
            }
        }
        print("SEND: ", json.dumps(req, indent=4))
        self.ws.send(json.dumps(req))

        str = self.ws.recv()
        print("RECV: ", str)
        res = json.loads(str)
        self.assertEqual(res['Header']['MessageType'], "GW_LoginResponse")
        self.assertEqual(res['Header']['ClientHandle'], "client-handle")
        self.assertEqual(res['Header']['StatusCode'], "0")
        self.assertIsNotNone(res['Body']['SessionId'])
        sessionId2 = res['Body']['SessionId']

        #
        # receive session status notify for second session
        #
        str = self.ws.recv()
        print("RECV: ", str)
        res = json.loads(str)
        self.assertEqual(res['Header']['MessageType'], "GW_SessionStatusNotify")
        self.assertEqual(res['Header']['ClientHandle'], "client-handle")
        self.assertEqual(res['Header']['SessionId'], sessionId2)
        self.assertEqual(res['Body']['SessionStatus'], "Connect")

        #
        # send logout request for session 2 to close opc ua session
        #
        req = {
            "Header": {
                "MessageType": "GW_LogoutRequest",
                "ClientHandle": "client-handle",
                "SessionId": sessionId2
            },
            "Body": {
            }

        }
        print("SEND: ", json.dumps(req, indent=4))
        self.ws.send(json.dumps(req))

        str = self.ws.recv()
        print("RECV: ", str)
        res = json.loads(str)
        self.assertEqual(res['Header']['MessageType'], "GW_LogoutResponse")
        self.assertEqual(res['Header']['ClientHandle'], "client-handle")
        self.assertEqual(res['Header']['SessionId'], sessionId2)
        self.assertEqual(res['Header']['StatusCode'], "0")

    def test_anonymous(self):
        #
        # send login request to open opc ua session
        #
        req = {
            "Header": {
                "MessageType": "GW_LoginRequest",
                "ClientHandle": "client-handle"
            },
            "Body": {
                "DiscoveryUrl": self.OPC_SERVER_URL,
                "UserAuth": {
                    "Type": "Anonymous"
                }
            }
        }
        print("SEND: ", json.dumps(req, indent=4))
        self.ws.send(json.dumps(req))

        str = self.ws.recv()
        print("RECV: ", str)
        res = json.loads(str)
        self.assertEqual(res['Header']['MessageType'], "GW_LoginResponse")
        self.assertEqual(res['Header']['ClientHandle'], "client-handle")
        self.assertEqual(res['Header']['StatusCode'], "0")
        self.assertIsNotNone(res['Body']['SessionId'])
        sessionId = res['Body']['SessionId']

        #
        # receive session status notify
        #
        str = self.ws.recv()
        print("RECV: ", str)
        res = json.loads(str)
        self.assertEqual(res['Header']['MessageType'], "GW_SessionStatusNotify")
        self.assertEqual(res['Header']['ClientHandle'], "client-handle")
        self.assertEqual(res['Header']['SessionId'], sessionId)
        self.assertEqual(res['Body']['SessionStatus'], "Connect")

        #
        # send logout request to close opc ua session
        #
        req = {
            "Header": {
                "MessageType": "GW_LogoutRequest",
                "ClientHandle": "client-handle",
                "SessionId": sessionId
            },
            "Body": {
            }

        }
        print("SEND: ", json.dumps(req, indent=4))
        self.ws.send(json.dumps(req))

        str = self.ws.recv()
        print("RECV: ", str)
        res = json.loads(str)
        self.assertEqual(res['Header']['MessageType'], "GW_LogoutResponse")
        self.assertEqual(res['Header']['ClientHandle'], "client-handle")
        self.assertEqual(res['Header']['SessionId'], sessionId)
        self.assertEqual(res['Header']['StatusCode'], "0")

    def test_username(self):
        #
        # send login request to open opc ua session
        #
        req = {
            "Header": {
                "MessageType": "GW_LoginRequest",
                "ClientHandle": "client-handle"
            },
            "Body": {
                "DiscoveryUrl": self.OPC_SERVER_URL,
                "SecurityMode": "SignAndEncrypt",
                "SecurityPolicyUri": "http://opcfoundation.org/UA/SecurityPolicy#Basic128Rsa15",
                "UserAuth": {
                    "Type": "UserName",
                    "UserName": "user1",
                    "Password": "password1",
                    "SecurityPolicyUri": "http://opcfoundation.org/UA/SecurityPolicy#Basic128Rsa15"
                }
            }
        }
        print("SEND: ", json.dumps(req, indent=4))
        self.ws.send(json.dumps(req))

        str = self.ws.recv()
        print("RECV: ", str)
        res = json.loads(str)
        self.assertEqual(res['Header']['MessageType'], "GW_LoginResponse")
        self.assertEqual(res['Header']['ClientHandle'], "client-handle")
        self.assertEqual(res['Header']['StatusCode'], "0")
        self.assertIsNotNone(res['Body']['SessionId'])
        sessionId = res['Body']['SessionId']

        #
        # receive session status notify
        #
        str = self.ws.recv()
        print("RECV: ", str)
        res = json.loads(str)
        self.assertEqual(res['Header']['MessageType'], "GW_SessionStatusNotify")
        self.assertEqual(res['Header']['ClientHandle'], "client-handle")
        self.assertEqual(res['Header']['SessionId'], sessionId)
        self.assertEqual(res['Body']['SessionStatus'], "Connect")

        #
        # send logout request to close opc ua session
        #
        req = {
            "Header": {
                "MessageType": "GW_LogoutRequest",
                "ClientHandle": "client-handle",
                "SessionId": sessionId
            },
            "Body": {
            }

        }
        print("SEND: ", json.dumps(req, indent=4))
        self.ws.send(json.dumps(req))

        str = self.ws.recv()
        print("RECV: ", str)
        res = json.loads(str)
        self.assertEqual(res['Header']['MessageType'], "GW_LogoutResponse")
        self.assertEqual(res['Header']['ClientHandle'], "client-handle")
        self.assertEqual(res['Header']['SessionId'], sessionId)
        self.assertEqual(res['Header']['StatusCode'], "0")
