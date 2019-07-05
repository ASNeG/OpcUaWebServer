from ..web_gateway_testcase import WebGatewayTestCase


class TestSessionCloseChannel(WebGatewayTestCase):

    def setUp(self):
        WebGatewayTestCase.setUp(self)

    def tearDown(self):
        pass

    def test_broken_channel(self):
        self.ws.close()