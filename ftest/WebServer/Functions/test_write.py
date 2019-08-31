import json
from opcua import Client
from opcua import ua

from ..web_server_testcase import WebServerTestCase


class TestWrite(WebServerTestCase):

    def setUp(self):
        WebServerTestCase.setUp(self)

        self.opcua_client = Client(self.OPC_SERVER_URL)
        self.opcua_client.connect()

    def tearDown(self):
        WebServerTestCase.tearDown(self)
        self.opcua_client.disconnect()

    def test_wrong_format(self):
        msg = {
            'Header': {
                'MessageType': 'WRITE_REQUEST',
                'ClientHandle': '1'
            },
            'Body': {
                'WRONG_FIELD': 'WRONG_VALUE'
            }
        }

        self.ws.send(json.dumps(msg))
        resp = json.loads(self.ws.recv())

        self.assertEqual('WRITE_RESPONSE', resp['Header']['MessageType'])
        self.assertEqual('BadAttributeIdInvalid', resp['Header']['StatusCode'])

    def test_variable_not_found(self):
        msg = {
            'Header': {
                'MessageType': 'WRITE_REQUEST',
                'ClientHandle': '1'
            },
            'Body': {
                'Variable': 'NOT_EXIST'
            }
        }

        self.ws.send(json.dumps(msg))
        resp = json.loads(self.ws.recv())

        print(resp)
        self.assertEqual('WRITE_RESPONSE', resp['Header']['MessageType'])
        self.assertEqual('BadNodeIdUnknown', resp['Body']['StatusCode'])

    def test_write_bool(self):
        node = self.opcua_client.get_node("ns=2;i=220")

        resp = self.write_('BooleanTest', 'true', type_id='1')
        self.assertEqual('', resp['Body'])
        self.assertTrue(node.get_value())

        resp = self.write_('BooleanTest', 'false', type_id='1')
        self.assertEqual('', resp['Body'])
        self.assertFalse(node.get_value())

    def test_write_int(self):
        node = self.opcua_client.get_node("ns=2;i=208")

        resp = self.write_('Int32Test', '555', '8')
        self.assertEqual('', resp['Body'])
        self.assertEqual(555, node.get_value())

        resp = self.write_('Int32Test', '-20', '8')
        self.assertEqual('', resp['Body'])
        self.assertEqual(-20, node.get_value())

    def test_read_float(self):
        node = self.opcua_client.get_node("ns=2;i=216")

        resp = self.write_('FloatTest', '88', '11')
        self.assertEqual('', resp['Body'])
        self.assertEqual(88, node.get_value())

        resp = self.write_('FloatTest', '10.56', '11')
        self.assertEqual('', resp['Body'])
        self.assertEqual(10.56, node.get_value())

    def write_(self, variable, value, type_id):
        msg = {
            'Header': {
                'MessageType': 'WRITE_REQUEST',
                'ClientHandle': '1'
            },
            'Body': {
                'Variable': variable, 'Value': {
                    'Value': {'Body': value, 'Type': type_id}
                }
            }
        }

        print(msg)

        self.ws.send(json.dumps(msg))
        return json.loads(self.ws.recv())
