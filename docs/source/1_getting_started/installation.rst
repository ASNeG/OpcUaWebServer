Installation
====================

OpcUaWebServer provides different ways of installation.


Source Code
--------------

In order to compile and install the OpcUaWebServer from the source code, you should meet
the following requirements:

* `ASNeG OPC UA Stack`_ >= 4.0.0
* `ASNeG Demo Server`_ >= 4.0.0 - Optional
* CMake
* C++ compiler with C++11 support

The ASNeG Demo Server is only used for testing the OpcUaWebServer. For this reason the 
installation of the ASNeG Demo Server is optional.


**Linux (Ubuntu or Debian)**

To install the requirments, follow the instructions in the following documents.

* `Installation OpcUaStack <https://opcuastack.readthedocs.io/en/release4/1_getting_started/installation.html>`_.
* `Installation ASNeG Demo Server <https://asnegdemoserver.readthedocs.io/en/release4/1_getting_started/installation.html>`_. 

The next stept is the compilation of the source code and its installation. You should 
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

In order to compile the project, you should install MSBuild Tools, then run in the environment which
is suitable for your target platform (e.g. Native x86) the following command:

::

  $ build.bat -t local

If you would like to build x64 version you should set the suitable CMake generator for your environment 
with option *-vs*. For example:

::

  $ build.bat -t local -vs "Visual Studio 15 2017 Win64"


By default, the server is istalled into *C:\\ASNeG* directory. You also can change it with option *-i*:

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

todo


RPM Packages
-------------

todo


MSI Packages
--------------

todo


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

