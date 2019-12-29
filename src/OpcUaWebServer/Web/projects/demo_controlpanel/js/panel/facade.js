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
//  Agent
// ##############################################

function Facade(html_ul_id, html_status_icon_id, html_board_id, html_dialog_id) {

    // **********************************************
    //  private

    var variableList_ = [];

    var logger_ = new Logger("Facade");

    var config_;
    var view_;
    var toolfactory_;
    var parser_;
    var asnegClient_; 

    // **********************************************
    //  public Functions

    // ##############################################
    //  init
    // ##############################################

    this.init = function() {
        view_ = new View(handleClickFromListPanel, callbackWriteEventHandler, callbackFromDialog);
        toolfactory_ = new ToolFactory();
        parser_ = new Parser();
        config_ = new Config();
        config_.init(callbackLoadConfigComplete);
    }

    // **********************************************
    //  private Functions

    // ##############################################
    //  callbacks
    // ##############################################

    function callbackLoadConfigComplete(status) {
        if (status != "Success") {
            logger_.log("callbackLoadConfigComplete", status);
            return;
        }  

        // start Client
        asnegClient_ = new ASNeG_Client(getIp(), "json", callbackReveiceFromServer, callbackASNeGClientConnectionStatus);      
    }

    function callbackReveiceFromServer(responseObj) {
    
        if (responseObj.connectionStatusCode != null) {
            logger_.error("callbackReveiceFromServer", 
                responseObj.connectionStatusCode + " by " + responseObj.clientHandle
            );          
            if (variableList_[responseObj.clientHandle].status != responseObj.connectionStatusCode) {
                variableList_[responseObj.clientHandle].status = responseObj.connectionStatusCode;
                view_.setStatusOnCards(variableList_[responseObj.clientHandle], responseObj.connectionStatusCode);
            }
            return; 
        }

        try {
            switch (responseObj.msgType) {
                case "VALUELIST_RESPONSE" :
                    if (responseObj.value != null) {
                        asnegClient_.readValueInfos(responseObj.value);
                    } else {
                        logger_.error("callbackReveiceFromServer", "VALUELIST_RESPONSE=Status:" + responseObj.valueStatusCode);
                    }
                    break;
                case "VALUEINFO_RESPONSE" :
                    if (responseObj.value != null) {
                        // build VariableList
                        var newVariableArray = parser_.buildVariables(responseObj.value);                        
                        parser_.mergeToolsIntoNewList(variableList_, newVariableArray);
                        view_.removeOldCards(html_board_id, variableList_, newVariableArray);
                        view_.displayVariableList(html_ul_id, newVariableArray);
                        variableList_ = newVariableArray;

                        // create default Display
                        defaultDisplay();
                    } 
                    break;
                case "WRITE_RESPONSE" :
                    var color = "#f00";
                    if (responseObj.valueStatusCode == "Success") {
                        color = "#0f0";
                    } 
                    view_.setStatusInTable(responseObj.clientHandle, responseObj.valueStatusCode, color);
                    break;
                case "MONITORSTART_RESPONSE" :
                    variableList_[responseObj.clientHandle].status = responseObj.valueStatusCode;
                    view_.setStatusOnCards(variableList_[responseObj.clientHandle], responseObj.valueStatusCode);
                    break;    
                case "MONITORUPDATE_MESSAGE" :
                    if (responseObj.valueStatusCode == "Success") {
                        var variable = variableList_[responseObj.clientHandle];
                        responseObj.value = parser_.cast(responseObj.clientHandle, responseObj.value);

                        if (variable == null || responseObj.value == null) {
                            return;
                        }
                        console.log(responseObj);

                        var i;
                        for (i=0; i<variable.tools.length; i++) {                            
                            variable.tools[i].setValue(responseObj.value);
                        }

                        view_.setTimestampOnCards(variable, responseObj.sourceTimestamp);   
                    } 

                    if (variableList_[responseObj.clientHandle].status != responseObj.valueStatusCode) {                        
                        variableList_[responseObj.clientHandle].status = responseObj.valueStatusCode;                        
                        view_.setStatusOnCards(variable, responseObj.valueStatusCode);
                    }
                    break;  
                case "MONITORSTOP_RESPONSE" :
                    logger_.log("callbackReveiceFromServer", 
                        "MONITORSTOP_RESPONSE=" + responseObj.clientHandle + " :: " + responseObj.valueStatusCode);
                    break;
                case "READ_RESPONSE" :
                    logger_.log("callbackReveiceFromServer", 
                       "READ_RESPONSE=" + responseObj.clientHandle + " :: " + responseObj.valueStatusCode + " = " + responseObj.value);
                    break;
                case "HISTORICALREAD_RESPONSE" :
                    logger_.log("callbackReveiceFromServer", 
                        "HISTORICALREAD_RESPONSE=" + responseObj.clientHandle + " :: " + responseObj.valueStatusCode);
                    logger_.log(responseObj.value);
                    break;
                default :
                    logger_.error("callbackReveiceFromServer", "Unkown msgType=" + responseObj.msgType);
            }
        } catch (error) {
            logger_.error("callbackReveiceFromServer", error);
        }
    }

    function callbackASNeGClientConnectionStatus(status) {       
        var status_icon = document.getElementById(html_status_icon_id);
        if (status_icon == null) {
            logger_.log("callbackASNeGClientStatus",  "StatusChange=" + status);
            return;
        }
           
        status_icon.title = "Verbindungsstatus: " + status;
        switch (status) {
            case "OPEN" : 
                status_icon.src = "./pics/icon_online.png";
                var key;
                for (key in variableList_) {
                    if (variableList_[key].tools.length != 0) {
                        view_.setStatusOnCards(variableList_[key], "Success");
                    }
                }                
                break;
            case "CONNECTING" :
            case "CLOSING" :
            case "CLOSED" :
                status_icon.src = "./pics/icon_offline.png";
                var key;
                for (key in variableList_) {
                    if (variableList_[key].tools.length != 0) {
                        view_.setStatusOnCards(variableList_[key], status);
                    }
                }
                break;
        }
    }

    function callbackWriteEventHandler(valueName) {
        var value = view_.getValueFromTable(valueName);
        if (value != null) {
            view_.setStatusInTable(valueName, "schreibe...", "#fff");
            asnegClient_.writeEvent(valueName, value);
        }
    }

    function callbackFromDialog(valueName, displayObj) {
        // close Dialog Panel
        view_.closeDialogPanel(html_dialog_id);
        
        // create a new card
        var configObj = config_.getConfigObj(variableList_[valueName], displayObj);
        buildCard(valueName, configObj);
    }

    // ##############################################
    //  handle click events
    // ##############################################

    function handleClickCloseCard(valueName, cardId) {
        var variable = variableList_[valueName];
        view_.removeCard(html_board_id, cardId, variable);
        removeToolFromVariable(variable, cardId);
    }

    function handleClickCloseTableRow(valueName, cardId) {
        var variable = variableList_[valueName];
        view_.removeVariableFromTable(html_board_id, cardId, variable);
        removeToolFromVariable(variable, cardId);
    }

    function handleClickFromListPanel(valueName) {
        // close Navi
        document.getElementsByClassName("w3-sidenav")[0].style.display = "none";
        
        // show Dialog
        var displayObjs = config_.getDisplayToolList(valueName);
        view_.showDialogPanel(html_dialog_id, valueName, displayObjs);
    }

    // ##############################################
    //  build cards
    // ##############################################

    function buildCard(valueName, configObj) {       
        if (configObj == null) {
            logger_.error("buildCard",  "configObj is null");
            return;
        }

        var variable = variableList_[valueName];

        var toolObj;
        if (configObj.toolType == ToolType.TABLE) {
            toolObj = view_.addVariableToTable(html_board_id, variable, handleClickCloseTableRow);
        } else {
            configObj = view_.createCard(html_board_id, variable, configObj, handleClickCloseCard);
            toolObj = toolfactory_.create(configObj);
            if (toolObj == null) return null;
            toolObj.id = configObj.id;
        }
           
        variableList_[valueName].tools.push(toolObj);

        view_.changeCounterListPanel(valueName, variableList_[valueName].tools.length);
        view_.setStatusOnCards(variableList_[valueName], variableList_[valueName].status);

        if (variableList_[valueName].tools.length == 1) {
            asnegClient_.startMonitoring(valueName);
        }
    }

    // ##############################################
    //  helper functions
    // ##############################################

    function removeToolFromVariable(variable, cardId) {
        // remove Gauge from Variable       
        var i;
        var tmpList = [];
        for (i=0; i<variable.tools.length; i++) {
            if (variable.tools[i].id != cardId) {
                tmpList.push(variable.tools[i]);
            } else {
                if (variable.tools[i].type == ToolType.LINE) {
                    variable.tools[i].stop();
                }
            }
        }
        variableList_[variable.valueName].tools = tmpList;        

        view_.changeCounterListPanel(variable.valueName, 
            variableList_[variable.valueName].tools.length);

        // check stop Monitoring
        if (variableList_[variable.valueName].tools.length == 0) {            
            asnegClient_.stopMonitoring(variable.valueName);
        }
    }

    function getIp() {
        if (location.hostname == null || location.hostname == "") {
            return "wss://127.0.0.1:8081";
        } else {
            return "wss://" + location.hostname  + ":8081";
        }
    }

    var init = true;
    function defaultDisplay() {
        if (init) {
            init = false;
            var settings = config_.getSettingsFromDefaultTools(variableList_);
            var valueName;
            for (valueName in settings) {
                var configObjs = settings[valueName];
                var i;
                for (i=0; i<configObjs.length; i++) {
                    buildCard(valueName, configObjs[i]);
                }
            }
        }
    }
}
