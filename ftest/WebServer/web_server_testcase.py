import os
import unittest
import json
import time
import ssl
from websocket import create_connection


class WebServerTestCase(unittest.TestCase):

    def setUp(self):
        self.WSS_SERVER_URL = os.getenv('WSS_SERVER_URL', "wss://127.0.0.1:8081")
        self.WS_SERVER_URL = os.getenv('WS_SERVER_URL', "ws://127.0.0.1:8081")
        self.OPC_SERVER_URL = os.getenv('OPC_SERVER_URL', "opc.tcp://127.0.0.1:8889")
        self.SERVER_PKI_ROOT_DIR = os.path.join(os.getenv('SERVER_PKI_ROOT_DIR', '/tmp/'),
                                                'etc/OpcUaStack/ASNeG-Demo/pki')

        # TODO: Here the certs of client and server should be exchanged

        #
        # open web socket connection
        #
        if self.WSS_SERVER_URL:
            self.ws = create_connection(
                self.WSS_SERVER_URL,
                sslopt={
                     "cert_reqs" : ssl.CERT_NONE,
                     "check_hostname" : False,
                     "ssl_version" : ssl.PROTOCOL_TLSv1 
                }
            )
        else:
            self.ws = create_connection(self.WS_SERVER_URL)


    def tearDown(self):
        #
        # close web socket connection
        #
        self.ws.close()
