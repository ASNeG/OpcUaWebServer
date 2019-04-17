OpcUaWebServer
========================================================

Installation
--------------------------------------------------------

In order to install OpcUaWebServer in your machine type the following command:

**On Linux** 

::
    $ sh build.sh -t local
	 
	
**On Windows**

::
    $ build.bat local


Docker
--------------------------------------------------------

You can use Docker to run OpcUaWebServer:

::
    $ docker build -t OpcUaWebServer:latest .
    $ docker run -d -p 8890:8890 OpcUaWebServer:latest
