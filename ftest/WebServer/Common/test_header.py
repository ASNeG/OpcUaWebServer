import json

from ..web_server_testcase import WebServerTestCase


class TestCommonCommunication(WebServerTestCase):

    def setUp(self):
        WebServerTestCase.setUp(self)

    def tearDown(self):
        WebServerTestCase.tearDown(self)

    def test_client_handle(self):
        msg = {
            'Header': {
                'MessageType': 'VALUELIST_REQUEST',
                'ClientHandle': 'yyyxxxqqq'
            },
            'Body': {}
        }

        self.ws.send(json.dumps(msg))
        resp = json.loads(self.ws.recv())
        self.assertEqual(msg['Header']['ClientHandle'], resp['Header']['ClientHandle'])

    def test_wrong_message_type(self):
        msg = {
            'Header': {
                'MessageType': 'WRONG_REQUEST',
                'ClientHandle': '1'
            },
            'Body': {}
        }

        self.ws.send(json.dumps(msg))
        resp = json.loads(self.ws.recv())

        self.assertEqual('ERROR', resp['Header']['MessageType'])
        self.assertEqual('BadRequestHeaderInvalid', resp['Header']['StatusCode'])
