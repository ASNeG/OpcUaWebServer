FROM asneg/opcuastack:release-4.1.0

# Prepare workdir
ADD / /tmp
RUN mkdir /build
WORKDIR /build

# Build
RUN cmake /tmp/src
RUN make
RUN make install

# Start OpcUaWebServer
EXPOSE 8890 4840
CMD ["OpcUaServer3", "/etc/OpcUaStack/OpcUaWebServer/OpcUaServer.xml"]
