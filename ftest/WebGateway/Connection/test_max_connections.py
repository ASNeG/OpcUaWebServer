import json
import os
import unittest
from websocket import create_connection

class TestMaxConnections(unittest.TestCase):

    def test_max_connections(self):
        self.ws = []
        self.WS_SERVER_URL = os.getenv('WS_SERVER_URL', "ws://127.0.0.1:8082")
  

        #
        # open 100 web socket connection
        #
        print("open 100 connectins")
        for idx in range(0, 100, 1):
            try:
                local_ws = create_connection(self.WS_SERVER_URL, timeout=1)
            except:
                print("timeout error")
                self.assertTrue(False)
            self.ws.append(local_ws);
            print("CONNECT: ", idx, self.ws[idx])


        #
        # test max connection limits
        #
        print("test max connection limit")
        timeout = False
        try:
            ws = create_connection(self.WS_SERVER_URL, timeout=1)
        except:
            timeout = True
        self.assertTrue(timeout)

	#
	# close one conntion
        #
        print("close first connection")
        self.ws[0].close()

	#
        # test connect
        #
        print("open connection")
        timeout = False
        try:
            self.ws[0] = create_connection(self.WS_SERVER_URL, timeout=1)
        except:
            timeout = True
        self.assertFalse(timeout)

        #
        # close all connections
        #
        print("close all connections")
        for idx in range(0, 100, 1):
            self.ws[idx].close()

        
