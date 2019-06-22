#!/usr/bin/python3

from websocket import create_connection
import Check as c

ws = create_connection("ws://127.0.0.1:8082")
ws.ping("xx")

[opcode, str] = ws.recv_data(True)
print("RECV: ", opcode, " ", str)
c.checkEqual(opcode, 10)

ws.close()
