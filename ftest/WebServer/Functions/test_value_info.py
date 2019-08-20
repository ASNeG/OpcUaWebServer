import json

from ..web_server_testcase import WebServerTestCase


class TestValueInfo(WebServerTestCase):

    def setUp(self):
        WebServerTestCase.setUp(self)

    def tearDown(self):
        WebServerTestCase.tearDown(self)

    def test_wrong_format(self):
        msg = {
            'Header': {
                'MessageType': 'VALUEINFO_REQUEST',
                'ClientHandle': '1'
            },
            'Body': {
                'WRONG_FIELD': 'WRONG_VALUE'
            }
        }

        self.ws.send(json.dumps(msg))
        resp = json.loads(self.ws.recv())

        self.assertEqual('VALUEINFO_RESPONSE', resp['Header']['MessageType'])
        self.assertEqual('BadAttributeIdInvalid', resp['Header']['StatusCode'])

    def test_variable_not_found(self):
        msg = {
            'Header': {
                'MessageType': 'VALUEINFO_REQUEST',
                'ClientHandle': '1'
            },
            'Body': {
                'Variables': ['NOT_EXIST']
            }
        }

        self.ws.send(json.dumps(msg))
        resp = json.loads(self.ws.recv())

        self.assertEqual('VALUEINFO_RESPONSE', resp['Header']['MessageType'])
        self.assertEqual('BadNodeIdUnknown', resp['Body']['Variables'][0]['StatusCode'])

    def test_value_info(self):
        msg = {
            'Header': {
                'MessageType': 'VALUEINFO_REQUEST',
                'ClientHandle': '1'
            },
            'Body': {'Variables': ['BooleanTest', 'ByteArrayTest']}
        }

        self.ws.send(json.dumps(msg))
        resp = json.loads(self.ws.recv())

        self.assertEqual('VALUEINFO_RESPONSE', resp['Header']['MessageType'])

        variables = resp['Body']['Variables']
        self.assertEqual(2, len(variables))
        self.assertEqual('BooleanTest', variables[0]['Variable'])
        self.assertEqual('Boolean', variables[0]['Type'])
        self.assertEqual('false', variables[0]['IsArray'])

        self.assertEqual('ByteArrayTest', variables[1]['Variable'])
        self.assertEqual('Byte', variables[1]['Type'])
        self.assertEqual('true', variables[1]['IsArray'])

    def test_meta_data(self):
        msg = {
            'Header': {
                'MessageType': 'VALUEINFO_REQUEST',
                'ClientHandle': '1'
            },
            'Body': {'Variables': ['Boolean']}
        }

        self.ws.send(json.dumps(msg))
        resp = json.loads(self.ws.recv())

        variables = resp['Body']['Variables']
        self.assertEqual(1, len(variables))

        # TODO: Trim the value
        self.assertEqual(' Switch', variables[0]['MetaData']['DisplayName'])
