import json
import os
import unittest
import ssl
from websocket import create_connection

class TestMultipleWebSocketConnections(unittest.TestCase):

    def test_multiple_websocket_connections(self):
        self.ws = []
        self.sessionId = []
        self.WSS_GATEWAY_URL = os.getenv('WSS_GATEWAY_URL', "wss://127.0.0.1:8082")
        self.WS_GATEWAY_URL = os.getenv('WS_GATEWAY_URL', "ws://127.0.0.1:8082")
        self.OPC_SERVER_URL = os.getenv('OPC_SERVER_URL', "opc.tcp://127.0.0.1:8889")

        #
        # open 5 web socket connection
        #
        print("open 5 connections")
        for idx in range(0, 5, 1):
            if self.WSS_GATEWAY_URL:
                try:
                    local_ws = create_connection(
                        self.WSS_GATEWAY_URL,
                        sslopt={
                            "cert_reqs" : ssl.CERT_NONE,
                            "check_hostname" : False,
                            "ssl_version" : ssl.PROTOCOL_TLSv1 
                        }
                    )
                except:
                    print("wss timeout error ", idx)
                    self.assertTrue(False)
            else:
                try:
                    local_ws = create_connection(self.WS_GATEWAY_URL, timeout=1)
                except:
                    print("ws timeout error", idx)
                    self.assertTrue(False)
                
            self.ws.append(local_ws);
            print("CONNECT: ", idx, self.ws[idx])

        #
        # login
        #
        print("login")
        for idx in range(0, 5, 1):
            #
            # send login request to open opc ua session
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
            self.ws[idx].send(json.dumps(req))

            str = self.ws[idx].recv()
            print("RECV: ", str)
            res = json.loads(str)
            self.assertEqual(res['Header']['MessageType'], "GW_LoginResponse")
            self.assertEqual(res['Header']['ClientHandle'], "client-handle")
            self.assertEqual(res['Header']['StatusCode'], "0")
            self.assertIsNotNone(res['Body']['SessionId'])
            self.sessionId.append(res['Body']['SessionId'])

            #
            # receive session status notify
            #
            str = self.ws[idx].recv()
            print("RECV: ", str)
            res = json.loads(str)
            self.assertEqual(res['Header']['MessageType'], "GW_SessionStatusNotify")
            self.assertEqual(res['Header']['ClientHandle'], "client-handle")
            self.assertEqual(res['Header']['SessionId'], self.sessionId[idx])
            self.assertEqual(res['Body']['SessionStatus'], "Connect")


	#
	# read
        #
        print("read")
        for idx in range(0, 5, 1):
            #
            # send read request to the opc ua server
            #
            req = {
                "Header": {
                     "MessageType": "GW_ReadRequest",
                     "ClientHandle": "client-handle",
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
            self.ws[idx].send(json.dumps(req)) 
        
        
            #
            # receive read response from the opc ua server
            #
            str = self.ws[idx].recv()
            print("RECV: ", str)
            res = json.loads(str)
            self.assertEqual(res['Header']['MessageType'], "GW_ReadResponse")
            self.assertEqual(res['Header']['ClientHandle'], "client-handle")
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
            #
            # send logout request to close opc ua session
            #
            req = {
                "Header": {
                    "MessageType": "GW_LogoutRequest",
                    "ClientHandle": "client-handle",
                    "SessionId": self.sessionId[idx]
                },
                "Body": {
                }

            }
            print("SEND: ", json.dumps(req, indent=4))
            self.ws[idx].send(json.dumps(req))

            str = self.ws[idx].recv()
            print("RECV: ", str)
            res = json.loads(str)
            self.assertEqual(res['Header']['MessageType'], "GW_LogoutResponse")
            self.assertEqual(res['Header']['ClientHandle'], "client-handle")
            self.assertEqual(res['Header']['SessionId'], self.sessionId[idx])
            self.assertEqual(res['Header']['StatusCode'], "0")



        #
        # close all connections
        #
        print("close all connections")
        for idx in range(0, 5, 1):
            self.ws[idx].close()

        
