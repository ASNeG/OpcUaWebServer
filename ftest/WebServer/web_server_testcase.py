import os
import unittest
import json
import time
from websocket import create_connection


class WebServerTestCase(unittest.TestCase):

    def setUp(self):
        self.WS_SERVER_URL = os.getenv('WS_SERVER_URL', "ws://127.0.0.1:8081")
        self.OPC_SERVER_URL = os.getenv('OPC_SERVER_URL', "opc.tcp://127.0.0.1:8889")
        self.SERVER_PKI_ROOT_DIR = os.path.join(os.getenv('SERVER_PKI_ROOT_DIR', '/tmp/'),
                                                'etc/OpcUaStack/ASNeG-Demo/pki')

        # TODO: Here the certs of client and server should be exchanged

        #
        # open web socket connection
        #
        self.ws = create_connection(self.WS_SERVER_URL)

    def tearDown(self):
        #
        # close web socket connection
        #
        self.ws.close()