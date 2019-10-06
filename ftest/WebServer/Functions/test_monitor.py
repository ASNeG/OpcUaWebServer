import json
from opcua import Client
from opcua import ua

from ..web_server_testcase import WebServerTestCase


class TestMonitor(WebServerTestCase):

    def setUp(self):
        WebServerTestCase.setUp(self)

        self.opcua_client = Client(self.OPC_SERVER_URL)
        self.opcua_client.connect()

    def tearDown(self):
        WebServerTestCase.tearDown(self)
        self.opcua_client.disconnect()

    def test_wrong_start_format(self):
        msg = {
            'Header': {
                'MessageType': 'MONITORSTART_REQUEST',
                'ClientHandle': '1'
            },
            'Body': {
                'WRONG_FIELD': 'WRONG_VALUE'
            }
        }

        self.ws.send(json.dumps(msg))
        resp = json.loads(self.ws.recv())

        self.assertEqual('MONITORSTART_RESPONSE', resp['Header']['MessageType'])
        self.assertEqual('BadAttributeIdInvalid', resp['Header']['StatusCode'])

    def test_variable_not_found(self):
        resp = self.start_monitor_('NON_EXIST')
        self.assertEqual('MONITORSTART_RESPONSE', resp['Header']['MessageType'])
        self.assertEqual('BadNodeIdUnknown', resp['Body']['StatusCode'])

    def test_wrong_stop(self):
        resp = self.stop_monitor_('BooleanTest')
        self.assertEqual('MONITORSTOP_RESPONSE', resp['Header']['MessageType'])
        self.assertEqual('BadNoEntryExists', resp['Body']['StatusCode'])

    def test_update_message(self):
        print("set value <ns=2;i=220> to False")
        node = self.opcua_client.get_node("ns=2;i=220")
        node.set_value(ua.DataValue(False))

        print("start monitor")
        self.start_monitor_('BooleanTest')

        print("set value <ns=2;i=220> to True")
        node.set_value(ua.DataValue(True))

        print("read data from web server")
        resp = json.loads(self.ws.recv())
        print(resp)

        self.assertEqual('1', resp['Body']['Value']['Type'])
        self.assertEqual('true', resp['Body']['Value']['Body'])

        node.set_value(ua.DataValue(False))

        resp = json.loads(self.ws.recv())
        print(resp)

        self.assertEqual('false', resp['Body']['Value']['Body'])

        self.stop_monitor_('BooleanTest')

    def start_monitor_(self, variable):
        msg = {
            'Header': {
                'MessageType': 'MONITORSTART_REQUEST',
                'ClientHandle': '1'
            },
            'Body': {
                'Variable': variable
            }
        }
        self.ws.send(json.dumps(msg))
        resp = json.loads(self.ws.recv())
        print(resp)
        return resp

    def stop_monitor_(self, variable):
        msg = {
            'Header': {
                'MessageType': 'MONITORSTOP_REQUEST',
                'ClientHandle': '1'
            },
            'Body': {
                'Variable': variable
            }
        }
        self.ws.send(json.dumps(msg))
        resp = json.loads(self.ws.recv())
        print(resp)
        return resp
