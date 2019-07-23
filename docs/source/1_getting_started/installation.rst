Installation
====================

OpcUaWebServer provides different ways of installation.


Source Code
--------------

To compile and install the OpcUaWebServer from the source code, you should meet
the following requirements:

* `ASNeG OPC UA Stack`_ >= 4.0.0
* `ASNeG Demo Server`_ >= 4.0.0 - Optional
* CMake
* C++ compiler with C++11 support

The ASNeG Demo Server is only used for testing the OpcUaWebServer. For this reason the 
installation of the ASNeG Demo Server is optional.

To install the requirements, follow the instructions in the following documents.

* `Installation OpcUaStack <https://opcuastack.readthedocs.io/en/release4/1_getting_started/installation.html>`_.
* `Installation ASNeG Demo Server <https://asnegdemoserver.readthedocs.io/en/release4/1_getting_started/installation.html>`_. 


**Linux (Ubuntu or Debian)**

To compile the source code and install OpcUaWebServer locally, you should 
run in the root directory of the sources:

::

  $ sh build.sh -t local -s ASNEG_INSTALL_DIR 

*ASNEG_INSTALL_DIR* is the path where ASNeG OPC UA Stack is installed.

By default, the server is installed into *~/.ASNeG* directory. You can change it with
option *-i* (run *sh build.sh* for more details). 

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

To compile the project, you should install MSBuild Tools, then run in the environment which
is suitable for your target platform (e.g., Native x86) the following command:

::

  $ build.bat -t local

If you would like to build x64 version, you should set the suitable CMake generator for your environment with option *-vs*. For example:

::

  $ build.bat -t local -vs "Visual Studio 15 2017 Win64"


By default, the server is installed into *C:\\ASNeG* directory. You also can change it with option *-i*:

::

  $ build.bat -t local -i C:\another\path\to\install

Now the OpcUaWebServer can be started.

::
  
  $ set PATH=%PATH%;C:\ASNeG\bin
  $ set PATH=%PATH%;C:\ASNeG\lib
  $ OpcUaServer4 CONSOLE C:\ASNeG\etc\OpcUaStack\OpcUaWebServer\OpcUaServer.xml

This example is for the default case where the stack and the server are installed in *C:\\ASNeG* directory.


DEP Packages
--------------

You can install OpcUaWebServer by using a DEP package, which you can download `here <https://github.com/ASNeG/OpcUaWebServer/releases/`_ or build yourself by the following command:

::

  $ sh build.sh -t deb -s ASNEG_INSTALL_DIR 

Then OpcUaWebServer is available for installation by the command:

::

  $ dpkg -i OpcUaWebServer-x.y.z-x86_64.deb 

OpcUaWebServer will be installed as a service, and you can check its status typing:

::

  $ service OpcUaWebServer status


RPM Packages
-------------

Users of RPM distributives can install OpcUaWebServer by using an RPM package, which can be downloaded `here <https://github.com/ASNeG/OpcUaWebServer/releases/`_ or built by the following command:

::

  $ sh build.sh -t rpm -s ASNEG_INSTALL_DIR 

Then OpcUaWebServer is available for installing by the command:

::

  $ rpm -i OpcUaWebServer-x.y.z-x86_64.rpm 

 

MSI Packages
--------------

On Windows, OpcUaWebServer can be installed with MSI packages. The MSI packages belong to two kinds: a *usual* package without `ASNeG OPC UA Stack`_ and a *standalone* package including `ASNeG OPC UA Stack`_ and the all needed dependencies. The *usual* package is more suitable for a machine with several
OPC UA applications and you would like to update the stack and the dependencies for all of them. The *standalone* package is easy for distribution, so you
don't need to install anything except OpcUaWebServer.

You can download the packages `here <https://github.com/ASNeG/OpcUaWebServer/releases/`_ or build them.

To build the *usual* package, use the command:

::

  $ build.bat -t msi

For building the *standalone* one:

::

  $ build.bat -t msi -S

Docker
-----------

You can use Docker image instead of installing the web server on you machine:

:: 

  $ docker build -t OpcUaWebServer:latest . 
  $ docker run -d -p 8890:8890 -p 8080:8080 -p 8081:8081 -p 8082:8082 OpcUaWebServer:latest


If you want to run the server with the demo server as a data source, use *docker-compose*:

::

  $ docker-compose run -d -p 8080:8080 -p 8081:8081 -p 8082:8082 webserver

Then open the link http://127.0.0.1:8080 with your web browser.


References
-----------

* `ASNeG OPC UA Stack`_
* `ASNeG Demo Server`_

.. _`ASNeG OPC UA Stack`: https://asneg.github.io/projects/opcuastack
.. _`ASNeG Demo Server`: https://asneg.github.io/projects/asnegdemo

