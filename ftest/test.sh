#!/usr/bin

SCRIPT_PATH=$1

if [ -z "${SCRIPT_PATH}" ]
then
    nosetests -s -v $@
else
    nosetests -s -v ${SCRIPT_PATH}
fi
