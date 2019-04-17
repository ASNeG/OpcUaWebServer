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
//  View
// ##############################################

function View(handleClickFromListPanel, callbackWriteEventHandler, callbackFromDialog) {

    // **********************************************
    //  private

    var logger_ = new Logger("View");

    var tableList_ = [];

    var listPanel_ = new ListPanel(handleClickFromListPanel);
    var tablePanel_ = new TablePanel(callbackWriteEventHandler);
    var dialogPanel_ = new DialogPanel(callbackFromDialog);
    var dashboardPanel_ = new DashboardPanel();
    
    var tableHandler_ = new TableHandler();
    var dashboardHandler_ = new DashboardHandler();

    // **********************************************
    //  public

    // ##############################################
    //  list functions
    // ##############################################

    this.displayVariableList = function(html_ul_id, variableList) {
        // clear list
        listPanel_.clearList(html_ul_id);

        // draw new list
        var key;
        for (key in variableList) {
            if (variableList[key].isArray == false) {
                listPanel_.addVariable(
                    html_ul_id, 
                    variableList[key].valueName, 
                    variableList[key].metadata.displayName
                ); 
                listPanel_.changeStatus(
                    variableList[key].valueName, 
                    variableList[key].tools.length
                );
            }
        } 
    }

    this.changeCounterListPanel = function(valueName, length) {
        listPanel_.changeStatus(valueName, length);
    }

    // ##############################################
    //  dashboard functions
    // ##############################################

    this.removeOldCards = function(html_board_id, oldVariableList, newVariableList) {
        // check if all old variables exits
        var key;
        for (key in oldVariableList) {
            // find not existing variable
            if (newVariableList[key] == null) {                
                // close all open cards
                if (!oldVariableList[key].tools.length != 0) {
                    var i;              
                    for (i=0; i<oldVariableList[key].tools.length; i++) {
                        if (oldVariableList[key].tools[i].type == ToolType.TABLE) {
                            tablePanel_.removeVariable(oldVariableList[key].tools[i].id, oldVariableList[key].valueName);
                            removeTable(html_board_id, oldVariableList[key].tools[i].id);
                        } else {
                            dashboardPanel_.removeCard(html_board_id, oldVariableList[key].tools[i].id);
                        }
                    }
                }
            }
        }
    }

    this.createCard = function(html_board_id, variable, configObj, handleClickCloseCard) {       
        var cardId = dashboardPanel_.generateNewCardId();
        if (cardId == null) {
            logger_.error("createCard", "To many open cards.");
            return null;
        }

        var cardHeight = 336; // standart
        var cardWidth = 314;  // standart
        var countHeight = Math.ceil(configObj.height / cardHeight);
        var countWidth = Math.ceil(configObj.width / cardWidth);
        
        if (countHeight < 1 || countWidth < 1) {
            logger_.error("createCard", "Card size error.");
            return null;
        }
        
        var height = (cardHeight * countHeight) + (36 * (countHeight - 1));
        var width = (cardWidth * countWidth) + (36 * (countWidth - 1));       
        var cardStyle = "height:" + height + "px; " +
                        "width:" + width + "px;";
        var canvasStyle = "height:" + configObj.height + "px; " +
                          "width:" + configObj.width + "px;";

        configObj.cnv_id = dashboardPanel_.createCard(
            cardId, 
            variable.valueName, 
            variable.metadata.displayName,
            html_board_id, 
            cardStyle, 
            canvasStyle,
            handleClickCloseCard
        );
        
        if (configObj.cnv_id == null) {
            logger_.error("createCard", "Card cannot created.");
            return null;
        }
        
        configObj.id = cardId;

        return configObj;
    }

    this.removeCard = function(html_board_id, cardId, variable) {
        // remove Card
        dashboardPanel_.removeCard(html_board_id, cardId);
    }

    this.setStatusOnCards = function (variable, status) {
        var statusMsg = "Valuestatus: " + status;
        var icon = "./pics/icon_online.png";
        if (status != "Success") {
            icon = "./pics/icon_offline.png";     
        } 

        var i;
        for (i=0; i<variable.tools.length; i++) {
            if (variable.tools[i].type != ToolType.TABLE) {               
                dashboardHandler_.setStatus(variable.tools[i].id, statusMsg, icon);
            }
        }
    }

    this.setTimestampOnCards = function(variable, timestamp) {
        var i;
        for (i=0; i<variable.tools.length; i++) {
            if (variable.tools[i].type != ToolType.TABLE) {
                dashboardHandler_.setTimestamp(variable.tools[i].id, timestamp);
            }
        }
    }

    // ##############################################
    //  table functions
    // ##############################################

    this.addVariableToTable = function(html_board_id, variable, handleClickCloseTableRow) {
        // check if variable exits in table
        var i;
        for (i=0; i<variable.tools.length; i++) {
            if (variable.tools[i].type == ToolType.TABLE) {
                logger_.error("addVariableToTable", "variable exits in table");
                return null;
            }
        }

        var cardId = findTable(html_board_id);
        if (cardId == null) {
            logger_.error("addVariableToTable", "To many open cards.");
            return null;
        }

        tablePanel_.addVariable(cardId, variable.valueName, variable.metadata.displayName, handleClickCloseTableRow);
        listPanel_.changeStatus(variable.valueName, variable.tools.length + 1);

        var valueName = variable.valueName;
        var toolObj = new ToolObj();
        toolObj.type = ToolType.TABLE;
        toolObj.id = cardId;
        toolObj.setValue = function(value) {            
           tableHandler_.setValue(valueName, value);
        }

        return toolObj;
    }

    this.removeVariableFromTable = function(html_board_id, cardId, variable) {
        // Change List-Status
        listPanel_.changeStatus(variable.valueName, (variable.tools.length - 1));

        // table
        tablePanel_.removeVariable(cardId, variable.valueName);        
        removeTable(html_board_id, cardId);
    }

    this.setStatusInTable = function(valueName, status, color) {
        tableHandler_.setStatus(valueName, status, color);
    }

    this.getValueFromTable = function(valueName) {
        return tableHandler_.getValue(valueName);
    }

    // ##############################################
    //  Dialog Panel
    // ##############################################

    this.showDialogPanel = function(html_dialog_id, valueName, displayObjs) {
        dialogPanel_.show(html_dialog_id, valueName, displayObjs);
    }

    this.closeDialogPanel = function(html_dialog_id) {
        dialogPanel_.close(html_dialog_id);
    }

    // **********************************************
    //  private

    // ##############################################
    //  table functions
    // ##############################################

    function findTable(html_board_id) {
        // check all exists tables
        var i;
        for (i=0; i<tableList_.length; i++) {
            if (!tablePanel_.isTableFull(tableList_[i])) {
                return tableList_[i];
            }
        }

        // create new table
        var cardId = dashboardPanel_.generateNewCardId();
        if (cardId == null) {
            logger_.error("findTable", "To many open cards.");
            return null;
        }

        dashboardPanel_.createTableCard(html_board_id, cardId);
        tableList_.push(cardId);
        return cardId;
    }

    function removeTable(html_board_id, cardId) {
        if (tablePanel_.isTableEmpty(cardId)) {
            dashboardPanel_.removeCard(html_board_id, cardId);

            var tmpArry = [];
            var i;
            for (i=0; i<tableList_.length; i++) {
                if (tableList_[i] != cardId) {
                    tmpArry.push(tableList_[i]);
                }
            }
            tableList_ = tmpArry;
        }
    }
}
