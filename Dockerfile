FROM asneg/opcuastack:release-4


ADD / /code

# Prepare Python dependencies for tests
RUN apt-get update && apt-get install -y python3-pip
RUN pip3 install -r /code/ftest/requirements.txt

# Prepare workdir
RUN mkdir /build
WORKDIR /build

# Build
RUN cmake /code/src
RUN make -j 2
RUN make install

# Start OpcUaWebServer
EXPOSE 8890 4840 8080 8081 8082 22
CMD ["OpcUaServer4", "/etc/OpcUaStack/OpcUaWebServer/OpcUaServer.xml"]
