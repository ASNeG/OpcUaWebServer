Create Certificate
====================

The tool openssl can be used to generate a valid certificate. The following section shows
an example of how a certificate can be created on Ubuntu.

The first step is to create a new private key/pair. The following command line is used for
this. 

::

  $ openssl genrsa 2048 > privatekey.pem

The generated key is now in the filei privatkey.pem. 

The next step is to create a csr file. A CSR file is a file that you can send to a certificate 
authority to receive a digital server certificate.

::

  $ openssl req -new -key privatekey.pem -out csr.pem \
    -subj /C=DE/ST=Hessen/L=Neukirchen/O=ASNeG/OU=OPC\ UA\ Service\ Department/CN=ASNeG.de

You can pass the signing request on to a third party for signing or sign it yourself for 
development and testing.

We are now signing the certificate ourselves.

::

  $ openssl x509 -req -days 365 -in csr.pem -signkey privatekey.pem -out public.crt

The certificates created must still be entered in the configuration file.
