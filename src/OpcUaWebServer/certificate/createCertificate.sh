#!/bin/bash

#
# cleanup existing keys and certificates
#

#
# Create private key. This private key is required for the generation of the
# certificate request. 
#
echo "create private key..."
openssl genrsa 2048 > privatekey.pem


#
# Create csr file. A CSR file is a file that you can send to a certificaten
# authority to receive a digital server certificate.
#
echo "create csr file..."
openssl req -new -key privatekey.pem -out csr.pem \
    -subj /C=DE/ST=Hessen/L=Neukirchen/O=ASNeG/OU=OPC\ UA\ Service\ Department/CN=ASNeG.de


#
# You can pass the signing request on to a third party for signing or sign it 
# yourself for development and testing.
#
# We are now signing the certificate ourselves. 
#
echo "sign and create certificate..."
openssl x509 -req -days 365 -in csr.pem -signkey privatekey.pem -out public.crt

mv privatekey.pem websocket.pem
mv public.crt websocket.crt

echo "success"
