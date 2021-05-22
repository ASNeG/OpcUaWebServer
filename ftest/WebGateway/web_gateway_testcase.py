import os
import unittest
import json
import time
import ssl
import datetime
from websocket import create_connection

class WebGatewayTestCase(unittest.TestCase):

    def setUp(self):
        time.sleep(1)

        self.WSS_GATEWAY_URL = os.getenv('WSS_GATEWAY_URL', "wss://127.0.0.1:8082")
        self.WS_GATEWAY_URL = os.getenv('WS_GATEWAY_URL', "ws://127.0.0.1:8082")
        self.OPC_SERVER_URL = os.getenv('OPC_SERVER_URL', "opc.tcp://127.0.0.1:8889")
        self.SERVER_PKI_ROOT_DIR = os.path.join(os.getenv('SERVER_PKI_ROOT_DIR', '/tmp/'), 'etc/OpcUaStack/ASNeG-Demo/pki')

        # show time on display
        now = datetime.datetime.now()
        print("")
        print("*************************************************")
        print(self._testMethodName)
        print(now.strftime('%H:%M:%S'))

        #
        # open web socket connection
        #
        if self.WSS_GATEWAY_URL:
            self.ws = create_connection(
                self.WSS_GATEWAY_URL,
                sslopt={
                     "cert_reqs" : ssl.CERT_NONE,
                     "check_hostname" : False,
                     "ssl_version" : ssl.PROTOCOL_TLSv1 
                }
            )
        else:
            self.ws = create_connection(self.WS_GATEWAY_URL)


        #
        # send login request to open opc ua session
        #
        req = {
            "Header": {
                "MessageType": "GW_LoginRequest",
                "ClientHandle": self._testMethodName
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
        self.assertEqual(res['Header']['ClientHandle'], self._testMethodName)
        self.assertEqual(res['Header']['StatusCode'], "0")
        self.assertIsNotNone(res['Body']['SessionId'])
        self.sessionId = res['Body']['SessionId']

        #
        # receive session status notify
        #
        str = self.ws.recv()
        print("RECV: ", str)
        res = json.loads(str)
        self.assertEqual(res['Header']['MessageType'], "GW_SessionStatusNotify")
        self.assertEqual(res['Header']['ClientHandle'], self._testMethodName)
        self.assertEqual(res['Header']['SessionId'], self.sessionId)
        self.assertEqual(res['Body']['SessionStatus'], "Connect")

    def tearDown(self):
        #
        # send logout request to close opc ua session
        #
        req = {
            "Header": {
                "MessageType": "GW_LogoutRequest",
                "ClientHandle": "client-handle",
                "SessionId": self.sessionId
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
        self.assertEqual(res['Header']['SessionId'], self.sessionId)
        self.assertEqual(res['Header']['StatusCode'], "0")
        #
        # close web socket connection
        #
        self.ws.close()
