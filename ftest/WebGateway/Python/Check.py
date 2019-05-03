#!/usr/local/bion/python
import sys, os
from inspect import currentframe, getframeinfo

def checkExists(first, msg=None):
    try:
        first
    except NameError:
        first = None
    if first is None:
        frameinfo = getframeinfo(currentframe().f_back)
        print("checkExists error: ", first, "==", second)
        if msg != None:
            print("Message:          ", msg)
        print("Function:         ", frameinfo.filename)
        print("Line:             ", frameinfo.lineno)
        os._exit(1)

def checkEqual(first, second, msg=None):
    if first != second:
        frameinfo = getframeinfo(currentframe().f_back)
        print("checkEqual error: ", first, "==", second)
        if msg != None:
            print("Message:          ", msg)
        print("Function:         ", frameinfo.filename)
        print("Line:             ", frameinfo.lineno)
        os._exit(1)

def checkGreater(first, second, msg=None):
    if first <= second:
        frameinfo = getframeinfo(currentframe().f_back)
        print("checkGreater error: ", first, ">", second)
        if msg != None:
            print("Message:          ", msg)
        print("Function:         ", frameinfo.filename)
        print("Line:             ", frameinfo.lineno)
        os._exit(1)

def checkGreaterEQ(first, second, msg=None):
    if first < second:
        frameinfo = getframeinfo(currentframe().f_back)
        print("checkGreaterEQ error: ", first, ">=", second)
        if msg != None:
            print("Message:          ", msg)
        print("Function:         ", frameinfo.filename)
        print("Line:             ", frameinfo.lineno)
        os._exit(1)

def checkLesser(first, second, msg=None):
    if first >= second:
        frameinfo = getframeinfo(currentframe().f_back)
        print("checkLesser error: ", first, "<", second)
        if msg != None:
            print("Message:          ", msg)
        print("Function:         ", frameinfo.filename)
        print("Line:             ", frameinfo.lineno)
        os._exit(1)

def checkLesserEQ(first, second, msg=None):
    if first > second:
        frameinfo = getframeinfo(currentframe().f_back)
        print("checkLesserEQ error: ", first, "<=", second)
        if msg != None:
            print("Message:          ", msg)
        print("Function:         ", frameinfo.filename)
        print("Line:             ", frameinfo.lineno)
        os._exit(1)

def checkNotEqual(first, second, msg=None):
    if first == second:
        frameinfo = getframeinfo(currentframe().f_back)
        print("checkEqual error: ", first, "==", second)
        if msg != None:
            print("Message:          ", msg)
        print("Function:         ", frameinfo.filename)
        print("Line:             ", frameinfo.lineno)
        os._exit(1)


def checkIsInText(text, tag, msg=None):
    if not text.find(tag):
        frameinfo = getframeinfo(currentframe().f_back)
        print("check IsInText error: ", text, " ", tag)
        if msg != None:
            print("Message:          ", msg)
        print("Function:         ", frameinfo.filename)
        print("Line:             ", frameinfo.lineno)
        os._exit(1)


def checkIsNotInText(text, tag, msg=None):
    if text.find(tag):
        frameinfo = getframeinfo(currentframe().f_back)
        print("check IsInText error: ", text, " ", tag)
        if msg != None:
            print("Message:          ", msg)
        print("Function:         ", frameinfo.filename)
        print("Line:             ", frameinfo.lineno)
        os._exit(1)


