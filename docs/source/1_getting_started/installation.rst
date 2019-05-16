Installation
====================

OpcUaWebServer provides different ways of installation.


Source Code
--------------

In order to compile and install the OpcUaWebServer from the source code, you should meet
the following requirements:

* OpcUaStack >= 4.0.0
* ASNeG Demo Server >= 4.0.0

The ASNeG Demo Server is only used for testing the OpcUaWebServer. For this reason the 
installation of the ASNeG Demo Server is optional.


**Linux (Ubuntu or Debian)**

To install the requirments, follow the instructions in the following documents.

* `Installation OpcUaStack <https://opcuastack.readthedocs.io/en/release4/1_getting_started/installation.html>`_.
* `Installation ASNeG Demo Server <https://asnegdemoserver.readthedocs.io/en/release4/1_getting_started/installation.html>`_. 

The next stept is the compilation of the source code and its installation. You should 
run in the root directory of the sources:

::

  $ sh build.sh -t local -s ~/.ASNeG

Now the OpcUaWebServer can be started.

::
  
  $ export PATH=$PATH:~/.ASNeG/usr/bin
  $ export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:~/.ASNeG/usr/lib/
  $ OpcUaServer4 ~/.ASNeG/etc/OpcUaStack/OpcUaWebServer/OpcUaServer.xml

The OpcUaWebServer provides the following communication interfaces:

* HTTP interface on port 8080
* Web Socket Server on port 8081
* Web Socket Gateway on port 8082


**Windows**

todo


DEP Packages
--------------

todo


RPM Packages
-------------

todo


MSI Packages
--------------

todo


Docker
-----------

todo
