#!/usr/bin/python3

from websocket import create_connection

print("ConnectDisconnect")
ws = create_connection("ws://127.0.0.1:8082")
ws.close()
