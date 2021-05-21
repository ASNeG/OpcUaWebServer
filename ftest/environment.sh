#!/bin/bash

DOCKER_ENABLE="${DOCKER_ENABLE:=NO}"
if [ "${DOCKER_ENABLE}" = "YES" ] 
then
    #
    # We are running in a docker container
    #
    echo "DOCKER ENABLE"
    echo "============="
    export OPC_UA_PKI_DIR=ASNeG-Demo:/tmp/etc/OpcUaStack/ASNeG-Demo/pki:OpcUaWebServer:/tmp/etc/OpcUaStack/OpcUaWebServer/pki
else
    #
    # We are running on a local system
    #
    echo "DOCKER DISABLE"
    echo "=============="
fi

#
# trust  ASNeG-Demo certificate
#
echo ""
echo "OpcUaWebServer: trust ASNeG-Demo certificate"
echo "============================================"
echo "OpcUaCtrl4 appl_cert get ASNeG-Demo demo_server_cert.der demo_server_key.pem"
OpcUaCtrl4 appl_cert get ASNeG-Demo demo_server_cert.der demo_server_key.pem
echo "OpcUaCtrl4 cert del OpcUaWebServer all"
OpcUaCtrl4 cert del OpcUaWebServer all
echo "OpcUaCtrl4 cert add OpcUaWebServer demo_server_cert.der"
OpcUaCtrl4 cert add OpcUaWebServer demo_server_cert.der

#
# trust OpcUaWebServer certificate
#
echo ""
echo "ASNeG-Demo: trust OpcUaWebServer certificate"
echo "============================================"
echo "OpcUaCtrl4 appl_cert get OpcUaWebServer web_server_cert.der web_server_key.pem"
OpcUaCtrl4 appl_cert get OpcUaWebServer web_server_cert.der web_server_key.pem
echo "OpcUaCtrl4 cert del ASNeG-Demo all"
OpcUaCtrl4 cert del ASNeG-Demo all
echo "OpcUaCtrl4 cert add ASNeG-Demo web_server_cert.der"
OpcUaCtrl4 cert add ASNeG-Demo web_server_cert.der

#
# show application info
#
echo ""
echo "show application info"
echo "====================="

echo "OpcUaCtrl4 appl show"
OpcUaCtrl4 appl show

echo "OpcUaCtrl4 appl_cert show ASNeG-Demo"
OpcUaCtrl4 appl_cert show ASNeG-Demo
echo "OpcUaCtrl4 cert show ASNeG-Demo"
OpcUaCtrl4 cert show ASNeG-Demo
echo "OpcUaCtrl4 appl_cert show OpcUaWebServer"
OpcUaCtrl4 appl_cert show OpcUaWebServer
echo "OpcUaCtrl4 cert show OpcUaWebServer"
OpcUaCtrl4 cert show OpcUaWebServer

if [ "${DOCKER_ENABLE}" = "YES" ] 
then
    find /tmp -name \*
fi 
