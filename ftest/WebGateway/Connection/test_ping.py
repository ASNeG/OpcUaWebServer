import json

from ..web_gateway_testcase import WebGatewayTestCase


class TestPing(WebGatewayTestCase):

    def setUp(self):
        WebGatewayTestCase.setUp(self)

    def tearDown(self):
        WebGatewayTestCase.tearDown(self)

    def test_ping(self):
        self.ws.ping("xx")

        [opcode, str] = self.ws.recv_data(True)
        print("RECV: ", opcode, " ", str)
        self.assertEqual(opcode, 10)


