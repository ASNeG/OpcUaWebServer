import json
from opcua import Client
from opcua import ua

from ..web_server_testcase import WebServerTestCase


class TestRead(WebServerTestCase):

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
                'MessageType': 'READ_REQUEST',
                'ClientHandle': '1'
            },
            'Body': {
                'WRONG_FIELD': 'WRONG_VALUE'
            }
        }

        self.ws.send(json.dumps(msg))
        resp = json.loads(self.ws.recv())

        self.assertEqual('READ_RESPONSE', resp['Header']['MessageType'])
        self.assertEqual('BadAttributeIdInvalid', resp['Header']['StatusCode'])

    def test_variable_not_found(self):
        msg = {
            'Header': {
                'MessageType': 'READ_REQUEST',
                'ClientHandle': '1'
            },
            'Body': {
                'Variable': 'NOT_EXIST'
            }
        }

        self.ws.send(json.dumps(msg))
        resp = json.loads(self.ws.recv())

        print(resp)
        self.assertEqual('READ_RESPONSE', resp['Header']['MessageType'])
        self.assertEqual('BadNodeIdUnknown', resp['Body']['StatusCode'])

    def test_read_bool(self):
        node = self.opcua_client.get_node("ns=2;i=220")
        node.set_value(ua.DataValue(True))

        resp = self.read_('BooleanTest')
        print(resp)

        self.assertEqual('1', resp['Body']['Value']['Type'])
        self.assertEqual('true', resp['Body']['Value']['Body'])

        node.set_value(ua.DataValue(False))
        resp = self.read_('BooleanTest')
        self.assertEqual('false', resp['Body']['Value']['Body'])

    def test_read_int(self):
        node = self.opcua_client.get_node("ns=2;i=208")
        node.set_value(ua.DataValue(555))

        resp = self.read_('Int32Test')
        print(resp)

        self.assertEqual('8', resp['Body']['Value']['Type'])
        self.assertEqual('555', resp['Body']['Value']['Body'])

        node.set_value(ua.DataValue(-450))
        resp = self.read_('Int32Test')
        self.assertEqual('-450', resp['Body']['Value']['Body'])

    def test_read_float(self):
        node = self.opcua_client.get_node("ns=2;i=216")
        node.set_value(ua.DataValue(2.8))

        resp = self.read_('FloatTest')
        print(resp)

        self.assertEqual('11', resp['Body']['Value']['Type'])
        self.assertEqual('2.8', resp['Body']['Value']['Body'])

        node.set_value(ua.DataValue(-30.0))
        resp = self.read_('FloatTest')
        self.assertEqual('-30', resp['Body']['Value']['Body'])

    def read_(self, variable):
        msg = {
            'Header': {
                'MessageType': 'READ_REQUEST',
                'ClientHandle': '1'
            },
            'Body': {'Variable': variable}
        }

        print(msg)

        self.ws.send(json.dumps(msg))
        return json.loads(self.ws.recv())
