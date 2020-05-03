import json
import os
import unittest
import time
import ssl
from websocket import create_connection

class TestMaxConnections(unittest.TestCase):

    def test_max_connections(self):
        self.ws = []
        self.WSS_GATEWAY_URL = os.getenv('WSS_GATEWAY_URL', "wss://127.0.0.1:8082")
        self.WS_GATEWAY_URL = os.getenv('WS_GATEWAY_URL', "ws://127.0.0.1:8082")
  

        #
        # open 100 web socket connection
        #
        print("open 100 connections")
        for idx in range(0, 100, 1):
            try:
                if self.WSS_GATEWAY_URL:
                    local_ws = create_connection(
                        self.WSS_GATEWAY_URL,
                        timeout=3,
                        sslopt={
                            "cert_reqs" : ssl.CERT_NONE,
                            "check_hostname" : False,
                            "ssl_version" : ssl.PROTOCOL_TLSv1 
                        }
                    )
                else:
                    local_ws = create_connection(self.WS_GATEWAY_URL, timeout=3)
            except:
                print("timeout connect error")
                self.assertTrue(False)
            self.ws.append(local_ws)
            print("CONNECT: ", idx, self.ws[idx])
        time.sleep(1)


        #
        # test max connection limits
        #
        print("test max connection limit")
        timeout = False
        try:
            if self.WSS_GATEWAY_URL:
                self.ws[idx] = create_connection(
                    self.WSS_GATEWAY_URL,
                    timeout=3,
                    sslopt={
                        "cert_reqs" : ssl.CERT_NONE,
                        "check_hostname" : False,
                        "ssl_version" : ssl.PROTOCOL_TLSv1 
                    }
                )
            else:
                self.ws = create_connection(self.WS_GATEWAY_URL, timeout=3)
        except:
            print("timeout max connection ok")
            timeout = True
        self.assertTrue(timeout)

        #
        # close one connection
        #
        print("close first connection")
        self.ws[0].close()

        #
        # test connect
        #
        time.sleep(1)
        print("open connection")
        timeout = False
        try:
            if self.WSS_GATEWAY_URL:
                self.ws[0] = create_connection(
                    self.WSS_GATEWAY_URL,
                    timeout=3,
                    sslopt={
                        "cert_reqs" : ssl.CERT_NONE,
                        "check_hostname" : False,
                        "ssl_version" : ssl.PROTOCOL_TLSv1 
                    }
                )
            else:
                self.ws[0] = create_connection(self.WS_GATEWAY_URL, timeout=3)

        except:
            print("timeout connect error")
            timeout = True
        self.assertFalse(timeout)

        #
        # close all connections
        #
        print("close all connections")
        for idx in range(0, 100, 1):
            self.ws[idx].close()
            print("DISCONNECT: ", idx)

        time.sleep(3)
