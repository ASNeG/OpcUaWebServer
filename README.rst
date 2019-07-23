OpcUaWebServer
========================================================

.. image:: http://jenkins.asneg.de:8080/job/OpcUaWebServer/job/master/badge/icon
    :target: http://jenkins.asneg.de:8080/job/OpcUaWebServer/job/master/
.. image:: https://img.shields.io/github/downloads/ASNeG/OpcUaWebServer/total.svg
    :alt: Github All Releases
    :target: https://github.com/ASNeG/OpcUaWebServer
.. image:: https://asneg-invate-form.herokuapp.com/badge.svg
    :alt: Slack Community
    :target: https://asneg-invate-form.herokuapp.com



About
--------------------------------------------------------

ASNeG OPC UA Web Server is a web application to access
to OPC UA data via the Internet. It provides a JSON API via WebSocket protocol
and has an embedded OPC UA client to communicate with OPC UA servers. 


Features
--------------------------------------------------------

* Embedded **OPC UA Client** from `ASNeG OPC UA Stack`_
* **HTTP Server** to distribute to delivery the web pages
* **WebSocket Server** with a simplified JSON API
* **WebSocket Gateway** with a JSON-OPC UA mapping
* **Web Panel** to visualize the process data by using graphical JavaScript library
* Configuration with XML files
* Supports Linux and Windows

For more information see `Overview <https://opcuawebserver.readthedocs.io/en/latest/1_getting_started/overview.html>`_
section.

Requirements
--------------------------------------------------------

* ANSeG OPC UA Stack >= 4.0.0
* CMake
* C++ compiler with C++11 support

Installing
--------------------------------------------------------

Before install OpcUaWebServer you must `install ASNeG OpcUaStack`_. See this
tutorial for more information.

Then type the following command:

**On Linux** 

::

    $ sh build.sh -t local -s ASNEG_INSTALL_DIR
     
    
**On Windows**

::

    $ build.bat local -s ASNEG_INSTALL_DIR

*ASNEG_INSTALL_DIR* is the path where ASNeG OPC UA Stack is installed.

For more information see `Installation <https://opcuawebserver.readthedocs.io/en/latest/1_getting_started/installation.html>`_
section.

Usage
-------------------------------------------------------

If you have installed the server locally, you can run it by using
the following command:

On Linux

::
  
  $ OpcUaServer4 ~/ASNEG_INSTALL_DIR/etc/OpcUaStack/OpcUaWebServer/OpcUaServer.xml

On Windows

::

  $ OpcUaServer4 CONSOLE ASNEG_INSTALL_DIR\etc\OpcUaStack\OpcUaWebServer\OpcUaServer.xml

Also you can use Docker without installing any dependencies:

:: 

  $ docker build -t OpcUaWebServer:latest . 
  $ docker run -d -p 8890:8890 -p 8080:8080 -p 8081:8081 -p 8082:8082 OpcUaWebServer:latest

  
Demo Application
----------------------------------------------

The demo application contains the web server and a demo OPC UA server as a data source. You can found
it `here <http://jenkins.asneg.de:8083/>`_ or run on your machine:

::

  $ docker-compose run -d -p 8080:8080 -p 8081:8081 -p 8082:8082 webserver

Then open the link http://127.0.0.1:8080 with your web browser.

Development Status
----------------------------------------------

ASNeG OPC UA Web Server is in the development stage. Moreover it dependents on OpcUaStack 4, which
is also being developed and not released. 

References
----------------------------------------------

* `Project Homepage`_
* Documentation_
* `ASNeG OPC UA Stack`_


.. _`Project Homepage`: https://asneg.github.io/projects/opcuawebserver
.. _Documentation: https://opcuawebserver.readthedocs.io
.. _`ASNeG OPC UA Stack`: https://asneg.github.io/projects/opcuastack
.. _`install ASNeG OpcUaStack`: https://opcuastack.readthedocs.io/en/latest/1_getting_started/installation.html
