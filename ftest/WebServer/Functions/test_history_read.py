import json
from opcua import Client
from opcua import ua

from ..web_server_testcase import WebServerTestCase


class TestHistoricalRead(WebServerTestCase):

    def setUp(self):
        WebServerTestCase.setUp(self)

    def tearDown(self):
        WebServerTestCase.tearDown(self)

    def test_wrong_format(self):
        msg = {
            'Header': {
                'MessageType': 'HISTORICALREAD_REQUEST',
                'ClientHandle': '1'
            },
            'Body': {
                'WRONG_FIELD': 'WRONG_VALUE'
            }
        }

        self.ws.send(json.dumps(msg))
        resp = json.loads(self.ws.recv())

        self.assertEqual('HISTORICALREAD_RESPONSE', resp['Header']['MessageType'])
        self.assertEqual('BadAttributeIdInvalid', resp['Header']['StatusCode'])

    def test_variable_not_found(self):
        msg = {
            'Header': {
                'MessageType': 'HISTORICALREAD_REQUEST',
                'ClientHandle': '1'
            },
            'Body': {
                'Variable': 'NOT_EXIST',
                'StartTime': '20020131T225959',
                'EndTime': '20020131T235959'
            }
        }

        self.ws.send(json.dumps(msg))
        resp = json.loads(self.ws.recv())

        print(resp)
        self.assertEqual('HISTORICALREAD_RESPONSE', resp['Header']['MessageType'])
        self.assertEqual('BadNodeIdUnknown', resp['Body']['StatusCode'])

    def test_read_variable_wo_history(self):
        """TODO: Not implemented yet"""

    def test_read(self):
        """TODO: Not implemented yet"""

    def read_(self, variable):
        msg = {
            'Header': {
                'MessageType': 'HISTORICALREAD_REQUEST',
                'ClientHandle': '1'
            },
            'Body': {
                'Variable': variable,
                'StartTime': '20020131T225959',
                'EndTime': '20020131T235959'
            }
        }

        print(msg)

        self.ws.send(json.dumps(msg))
        return json.loads(self.ws.recv())
