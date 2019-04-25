/*
   Copyright 2015 Kai Huebl (kai@huebl-sgh.de)

   Lizenziert gemäß Apache Licence Version 2.0 (die „Lizenz“); Nutzung dieser
   Datei nur in Übereinstimmung mit der Lizenz erlaubt.
   Eine Kopie der Lizenz erhalten Sie auf http://www.apache.org/licenses/LICENSE-2.0.

   Sofern nicht gemäß geltendem Recht vorgeschrieben oder schriftlich vereinbart,
   erfolgt die Bereitstellung der im Rahmen der Lizenz verbreiteten Software OHNE
   GEWÄHR ODER VORBEHALTE – ganz gleich, ob ausdrücklich oder stillschweigend.

   Informationen über die jeweiligen Bedingungen für Genehmigungen und Einschränkungen
   im Rahmen der Lizenz finden Sie in der Lizenz.

   Autor: Samuel Huebl (samuel.huebl@asneg.de)
*/

// ##############################################
//  ASNeG WebSocket Client
// ##############################################
function ASNeG_Client(ip, format, callbackReveiceFromServer, callbackASNeGClientStatus) {

	// **********************************************
	//  public Functions

	// status web socket
	this.status = status;
	// read-write
	this.readEvent = readEvent;
	this.historicalReadEvent = historicalReadEvent;
	this.writeEvent = writeEvent;
	// monitoring
	this.startMonitoring = startMonitoring;
	this.stopMonitoring = stopMonitoring;
	// read ValueList/-Info
	this.readValueList = readValueList;
	this.readValueInfos = sendValueInfo;

	// **********************************************
	//  private

	var ws_;
	var monitoringVariableList_ = [];

	var logger_ = new Logger("ASNeG_Client");

	// init WebSocket
	initWebSocket();

	// **********************************************
	//  Client - Functions

	// ##############################################
	//  Create WebSocket
	// ##############################################

	function initWebSocket() {
		// Create a connection to Server
		connect();
	}

	// ##############################################
	//  Connection
	// ##############################################

	function connect() {
		ws_ = new WebSocket(ip, format);
		ws_.onopen = webSocket_onopen;
		ws_.onerror = webSocket_onerror;
		ws_.onmessage = webSocket_onmessage;
		ws_.onclose = webSocket_onclose;
		reconnect();
	}

	function reconnect() {
		statusChange();
		if (status() == "CLOSED") {
			connect();
		}
	}

	// ##############################################
	//  Value List
	// ##############################################

	function readValueList() {
		if (ws_.readyState == 1) { // OPEN
			var msg = {
				Header : {
					MessageType: 'VALUELIST_REQUEST',
					ClientHandle: 'VALUELIST'
				},
				Body : {
				}
			}
			ws_.send(JSON.stringify(msg));
		} else {
			logger_.log("readValueList", "Cannot send. Status=" + status());
		}	
	}

	// ##############################################
	//  Value List Info
	// ##############################################

	function sendValueInfo(variables) {
		if (ws_.readyState == 1) { // OPEN
			var msg = {
				Header : {
					MessageType: 'VALUEINFO_REQUEST',
					ClientHandle: 'VALUEINFO'
				},
				Body : {
					Variables: variables
				}
			}
			ws_.send(JSON.stringify(msg));
		} else {
			logger_.log("sendValueInfo", "Cannot send. Status=" + status());
		}	
	}

	// ##############################################
	//  Read Event
	// ##############################################

	function readEvent(clientHandle) {
		if (ws_.readyState == 1) { // OPEN
			var msg = {
				Header : {
					MessageType: 'READ_REQUEST',
					ClientHandle: clientHandle
				},
				Body : {
					Variable: clientHandle
				}
			}
			ws_.send(JSON.stringify(msg));
		} else {
			logger_.log("readEvent", "Cannot send. Status=" + status());
		}
	}

	// ##############################################
	//  Hirstorical Read Event
	// ##############################################

	function historicalReadEvent(clientHandle, startTime, endTime) {
		if (ws_.readyState == 1) { // OPEN
			var msg = {
				Header : {
					MessageType: 'HISTORICALREAD_REQUEST',
					ClientHandle: clientHandle
				},
				Body : {
					Variable: clientHandle,
					StartTime: dateToISO(startTime),
					EndTime: dateToISO(endTime)
				}
			}
			ws_.send(JSON.stringify(msg));
		} else {
			logger_.log("historicalRead", "Cannot send. Status=" + status());
		}
	}

	// ##############################################
	//  Write Event
	// ##############################################

	function writeEvent(clientHandle, value) {
		if (ws_.readyState == 1) { // OPEN
			var msg = {
				Header : {
					MessageType: 'WRITE_REQUEST',
					ClientHandle: clientHandle
				},
				Body : {
					Variable: clientHandle,
					SourceTimestamp: getSourceTimestamp(),
					Value: value
				}
			}
			ws_.send(JSON.stringify(msg));
		} else {
			logger_.log("writeEvent", "Cannot send. Status=" + status());
		}
	}

	// ##############################################
	//  Monitoring
	// ##############################################

	function startMonitoring(clientHandle) {
		if (ws_.readyState == 1) { // OPEN
			var msg = {
				Header : {
					MessageType: 'MONITORSTART_REQUEST',
					ClientHandle: clientHandle
				},
				Body : {
				   	Variable: clientHandle
				}
			}
			ws_.send(JSON.stringify(msg));
		} else {
			logger_.log("startMonitoring", "Cannot send. Status=" + status());
		}
	}

	function stopMonitoring(clientHandle) {
		if (ws_.readyState == 1) { // OPEN
			var msg = {
				Header : {
					MessageType: 'MONITORSTOP_REQUEST',
					ClientHandle: clientHandle
				},
				Body : {
				   	Variable: clientHandle
				}
			}
			ws_.send(JSON.stringify(msg));
		} else {
			logger_.log("stopMonitoring", "Cannot send. Status=" + status());
		}
	}

	function startOldMonitorVariables() {
		var i;
		for (i=0; i<monitoringVariableList_.length; i++) {
			startMonitoring(monitoringVariableList_[i]);			
		}		
	}

	function checkOldMonitorVariables(value) {
		var unknownVariables = [];
		var i;
		for (i=0; i<monitoringVariableList_.length; i++) {
			if (value.indexOf(monitoringVariableList_[i]) < 0) {
				unknownVariables.push(monitoringVariableList_[i]);
			}
		}

		for (i=0; i<unknownVariables.length; i++) {
			removeMonitoringVariableFromArray(unknownVariables[i]);
		}		
	}

	function removeMonitoringVariableFromArray(clientHandle) {
		var tmpArray = [];
		var i;
		for (i=0; i<monitoringVariableList_.length; i++) {
			if (monitoringVariableList_[i] != clientHandle) {
				tmpArray.push(monitoringVariableList_[i]);
			}
		}
		monitoringVariableList_ = tmpArray;
	}

	// ##############################################
	//  Status
	// ##############################################

	function status() {
		var status;
		
		switch (ws_.readyState) {
			case 0 : 
				status = "CONNECTING";
				break;
			case 1 : 
				status = "OPEN";
				break;
			case 2 : 
				status = "CLOSING";
				break;
			case 3 : 
				status = "CLOSED";
				break;
			default :
				status = "UNDEFINED: status=" + ws_.readyState;
		}

		return status;
	}

	function statusChange() {
		callbackASNeGClientStatus(status());
	}

	// ##############################################
	//  Timestamp
	// ##############################################

	function getLocalTimestamp(utc) {
		if (utc == null) return null;
		var utcDate = new Date(utc);
		var offsetInMinutes = utcDate.getTimezoneOffset();
		var targetDate = new Date(utcDate.getTime() - (offsetInMinutes * 60000)); //millisec 60*1000
		return targetDate.toLocaleString();
	}

	function getSourceTimestamp() {
		var utcTime = new Date().toUTCString();
    	var utcDate = new Date(utcTime);
		return utcDate.toISOString();
	}

	function dateToISO(date) {
		if (date == null) return null;
		var utcTime = new Date(date).toUTCString();
		var utcDate = new Date(utcTime);
		return utcDate.toISOString();
	}

	// **********************************************
	//  WebSocket - Functions

	// ##############################################
	//  WebSocket Open
	// ##############################################

	function webSocket_onopen()
	{
		logger_.log("webSocket_onopen", "WebSocket Verbindung geoeffnet");
		readValueList();
		statusChange();
	}

	// ##############################################
	//  WebSocket Close
	// ##############################################

	function webSocket_onclose(event) {
		statusChange();
		if (this.readyState == 2) { // CLOSING
			logger_.log("webSocket_onclose", "Schliesse Verbindung...");
			logger_.log("webSocket_onclose", "Die Verbindung durchlaeuft den Closing Handshake");
		}
		else if (this.readyState == 3) { // CLOSED
			logger_.log("webSocket_onclose", "Verbindung geschlossen...");
			logger_.log("webSocket_onclose", "Die Verbindung wurde geschlossen oder konnte nicht aufgebaut werden");
			logger_.log("webSocket_onclose", "Die Verbindung wird neu aufgebaut...");
			reconnect();
		}
		else {
			logger_.log("webSocket_onclose", "Verbindung geschlossen...");
			logger_.log("webSocket_onclose", "Nicht behandelter State: " + this.readyState);
		}
	}

	// ##############################################
	//  WebSocket receive message
	// ##############################################

	function webSocket_onmessage(message) {
		// logger_.log("webSocket_onmessage", "WebSocket Data: " + message.data);
		try {
			var data = JSON.parse(message.data);

			var responseObj = new ResponseObj();

			responseObj.msgType = data.Header.MessageType;
			responseObj.clientHandle = data.Header.ClientHandle;

			if (data.Header.StatusCode != undefined) { // Error
				logger_.error("webSocket_onmessage", "ConnectionError: Header-StatusCode=" + data.Header.StatusCode);
				responseObj.connectionStatusCode = data.Header.StatusCode;
				callbackReveiceFromServer(responseObj);
				return;
			}

			responseObj.sourceTimestamp = getLocalTimestamp(data.Body.SourceTimestamp);
			responseObj.serverTimestamp = getLocalTimestamp(data.Body.ServerTimestamp);
			switch (responseObj.msgType) {
				case "READ_RESPONSE" :
					if (data.Body.StatusCode != undefined) {
						responseObj.valueStatusCode = data.Body.StatusCode;
					} else {
						responseObj.valueStatusCode = "Success";
						responseObj.value = data.Body.Value;
					}
					break;
				case "WRITE_RESPONSE" :
					if (data.Body.StatusCode != undefined) {
						responseObj.valueStatusCode = data.Body.StatusCode;
					} else {
						responseObj.valueStatusCode = "Success";
					}
					break;
				case "VALUELIST_RESPONSE" :
					if (data.Body.StatusCode != undefined) {
						responseObj.valueStatusCode = data.Body.StatusCode;
					} else {
						responseObj.value = data.Body.Variables;
						responseObj.valueStatusCode = "Success";;
						checkOldMonitorVariables(responseObj.value);
					}
					break;
				case "VALUEINFO_RESPONSE" :
					if (data.Body.StatusCode != undefined) {
						responseObj.valueStatusCode = data.Body.StatusCode;
					} else {
						responseObj.valueStatusCode = "Success";
						responseObj.value = data.Body.Variables;
						startOldMonitorVariables();
					}
					break;	
				case "MONITORSTART_RESPONSE" :
					if (data.Body.StatusCode != undefined) {
						responseObj.valueStatusCode = data.Body.StatusCode;
					} else {
						responseObj.valueStatusCode = "Success";
						monitoringVariableList_.push(responseObj.clientHandle);					
					}
					break;
				case "MONITORSTOP_RESPONSE" :
					if (data.Body.StatusCode != undefined) {
						responseObj.valueStatusCode = data.Body.StatusCode;
					} else {
						responseObj.valueStatusCode = "Success";
					}
					removeMonitoringVariableFromArray(responseObj.clientHandle);
					break;
				case "MONITORUPDATE_MESSAGE" :
					if (data.Body.StatusCode != undefined) {
						responseObj.valueStatusCode = data.Body.StatusCode;
					} else {
						responseObj.valueStatusCode = "Success";
						responseObj.value = data.Body.Value.Body;
					}
					break;	
				case "HISTORICALREAD_RESPONSE" :
					if (data.Body.StatusCode != undefined) {
						responseObj.valueStatusCode = data.Body.StatusCode;
					} else {
						responseObj.valueStatusCode = "Success";
						responseObj.value = data.Body.DataValues;
					}
					break;
				default :
					logger_.error("webSocket_onmessage", "TypeError: unknown msgType=" + responseObj.msgType);
			}

			callbackReveiceFromServer(responseObj);

		} catch(error) {
			logger_.error("webSocket_onmessage", error);
		}
	}

	// ##############################################
	//  WebSocket Error
	// ##############################################

	function webSocket_onerror(event) {
		statusChange();
		var reason = event.reason;
		var code = event.code;
		logger_.log("webSocket_onerror", "WebSocketError: " + reason + "(" + code + ")");
	}
	
}

