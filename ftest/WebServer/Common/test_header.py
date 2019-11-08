import json

from ..web_server_testcase import WebServerTestCase


class TestCommonCommunication(WebServerTestCase):

    def setUp(self):
        WebServerTestCase.setUp(self)

    def tearDown(self):
        WebServerTestCase.tearDown(self)

    def test_client_handle(self):
        req = {
            'Header': {
                'MessageType': 'VALUELIST_REQUEST',
                'ClientHandle': 'yyyxxxqqq'
            },
            'Body': {}
        }

        print("SEND: ", json.dumps(req, indent = 4))
        self.ws.send(json.dumps(req)) 

        str = self.ws. recv()
        print("RECV: ", str)
        res = json.loads(str)
        self.assertEqual(req['Header']['ClientHandle'], res['Header']['ClientHandle'])

    def test_wrong_message_type(self):
        req = {
            'Header': {
                'MessageType': 'WRONG_REQUEST',
                'ClientHandle': '1'
            },
            'Body': {}
        }

        print("SEND: ", json.dumps(req, indent = 4))
        self.ws.send(json.dumps(req)) 

        str = self.ws. recv()
        print("RECV: ", str)
        res = json.loads(str)

        self.assertEqual('ERROR', res['Header']['MessageType'])
        self.assertEqual('BadRequestHeaderInvalid', res['Header']['StatusCode'])
