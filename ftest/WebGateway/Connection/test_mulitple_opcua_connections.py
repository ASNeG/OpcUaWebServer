import json
import os
import unittest
import ssl
from websocket import create_connection

class TestMultipleWebSocketConnections(unittest.TestCase):

    def test_multiple_websocket_connections(self):
        self.sessionId = []
        self.WSS_GATEWAY_URL = os.getenv('WSS_GATEWAY_URL', "wss://127.0.0.1:8082")
        self.WS_GATEWAY_URL = os.getenv('WS_GATEWAY_URL', "ws://127.0.0.1:8082")
        self.OPC_SERVER_URL = os.getenv('OPC_SERVER_URL', "opc.tcp://127.0.0.1:8889")

        #
        # open web socket connection
        #
        print("open connections")
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
            self.ws = create_connection(self.WS_GATEWAY_URL, timeout=1)

        #
        # login
        #
        print("login")
        for idx in range(0, 5, 1):

            client_handle = self._testMethodName + str(idx)
 
            #
            # send login request to open opc ua session
            #
            req = {
                "Header": {
                    "MessageType": "GW_LoginRequest",
                    "ClientHandle": client_handle
                },
                "Body": {
                    "DiscoveryUrl":  self.OPC_SERVER_URL
                }
            }
            print("SEND: ", json.dumps(req, indent=4))
            self.ws.send(json.dumps(req))

            json_str = self.ws.recv()
            print("RECV: ", json_str)
            res = json.loads(json_str)
            self.assertEqual(res['Header']['MessageType'], "GW_LoginResponse")
            self.assertEqual(res['Header']['ClientHandle'], client_handle)
            self.assertEqual(res['Header']['StatusCode'], "0")
            self.assertIsNotNone(res['Body']['SessionId'])
            self.sessionId.append(res['Body']['SessionId'])

            #
            # receive session status notify
            #
            json_str = self.ws.recv()
            print("RECV: ", json_str)
            res = json.loads(json_str)
            self.assertEqual(res['Header']['MessageType'], "GW_SessionStatusNotify")
            self.assertEqual(res['Header']['ClientHandle'], client_handle)
            self.assertEqual(res['Header']['SessionId'], self.sessionId[idx])
            self.assertEqual(res['Body']['SessionStatus'], "Connect")


	#
	# read
        #
        print("read")
        for idx in range(0, 5, 1):
            client_handle = self._testMethodName + str(idx)


            #
            # send read request to the opc ua server
            #
            req = {
                "Header": {
                     "MessageType": "GW_ReadRequest",
                     "ClientHandle": client_handle,
                     "SessionId": self.sessionId[idx]
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
            json_str = self.ws.recv()
            print("RECV: ", json_str)
            res = json.loads(json_str)
            self.assertEqual(res['Header']['MessageType'], "GW_ReadResponse")
            self.assertEqual(res['Header']['ClientHandle'], client_handle)
            self.assertEqual(res['Header']['SessionId'], self.sessionId[idx])
            self.assertEqual(res['Header']['StatusCode'], "0")
            self.assertEqual(len(res['Body']['Results']), 2)
            self.assertIsNotNone(res['Body']['Results'][0]['Value'])
            self.assertIsNotNone(res['Body']['Results'][1]['Value'])


        #
        # logout
        #
        print("logout")
        for idx in range(0, 5, 1):

            client_handle = self._testMethodName + str(idx)

            #
            # send logout request to close opc ua session
            #
            req = {
                "Header": {
                    "MessageType": "GW_LogoutRequest",
                    "ClientHandle": client_handle,
                    "SessionId": self.sessionId[idx]
                },
                "Body": {
                }

            }
            print("SEND: ", json.dumps(req, indent=4))
            self.ws.send(json.dumps(req))

            json_str = self.ws.recv()
            print("RECV: ", json_str)
            res = json.loads(json_str)
            self.assertEqual(res['Header']['MessageType'], "GW_LogoutResponse")
            self.assertEqual(res['Header']['ClientHandle'], client_handle)
            self.assertEqual(res['Header']['SessionId'], self.sessionId[idx])
            self.assertEqual(res['Header']['StatusCode'], "0")



        #
        # close all connections
        #
        print("close connection")
        self.ws.close()

        
