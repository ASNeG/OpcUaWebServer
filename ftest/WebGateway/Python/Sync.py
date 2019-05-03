#!/usr/local/bion/python
import sys, time
import threading

import OpcUaClient as UAC

class SyncCall:


    def __init__(self, client):
        self.client = client
        self.res = [False]

    def call(self, objectNodeId, funcNodeId, *args):
        newArgs = [objectNodeId, funcNodeId]
        newArgs.extend(args)
        self.thread = threading.Thread(target=self.runCall, args=(newArgs))
        self.thread.start()

    def runCall(self, objectNodeId, funcNodeId, *args):
        self.res = self.client.callSync(objectNodeId, funcNodeId, *args)

    def wait(self, timeout=None):
        self.thread.join(timeout)
