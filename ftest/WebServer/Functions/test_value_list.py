import json

from ..web_server_testcase import WebServerTestCase


class TestValueList(WebServerTestCase):

    def setUp(self):
        WebServerTestCase.setUp(self)

    def tearDown(self):
        WebServerTestCase.tearDown(self)

    def test_value_list(self):
        msg = {
            'Header': {
                'MessageType': 'VALUELIST_REQUEST',
                'ClientHandle': '1'
            },
            'Body': {}
        }

        self.ws.send(json.dumps(msg))
        resp = json.loads(self.ws.recv())

        self.assertEqual('VALUELIST_RESPONSE', resp['Header']['MessageType'])
        self.assertEqual(112, len(resp['Body']['Variables']))
        self.assertIn('Boolean', resp['Body']['Variables'])
        self.assertIn('Int64', resp['Body']['Variables'])
        self.assertIn('LocalizedText', resp['Body']['Variables'])
