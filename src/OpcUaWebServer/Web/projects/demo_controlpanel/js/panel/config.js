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
//  Config
// ##############################################

function Config() {

	// **********************************************
	//  private

	var logger_ = new Logger("Config");

	var configTools_;
	var configVariables_;
	var configSettings_;
	var configDesign_;

	// **********************************************
	//  public Functions

	// ##############################################
	//  init
	// ##############################################

	this.init = function(initCompleteCallback) {
		readJSON("./json/tools.json"); // async!!
		readJSON("./json/variables.json"); // async!!
		readJSON("./json/settings.json"); // async!!
		readJSON("./json/design.json"); // async!!

		checkInitComplete(initCompleteCallback, 0, null); 
	}

	// ##############################################
	//  get Display Tool List
	// ##############################################

	this.getDisplayToolList = function(valueName) {
		var list = [];
		
		var variable = findValueNameInConfigVariables(valueName);
		if (variable == null) {
			return null;
		}

		var itToolId;
		var length = variable.toolids.length; 
		for (itToolId=0; itToolId<length; itToolId++) {
			var param = findIdInConfigTools(variable.toolids[itToolId]);
			if (param != null) {
				var displayObj = new DisplayObj();
				displayObj.id = param.id;
				displayObj.description = param.description;
				displayObj.icon = "./pics/" + param.icon;
				list.push(displayObj);
			}
		}

		return list;
	}	

	// ##############################################
	//  get Config Obj
	// ##############################################

	this.getConfigObj = function(variable, displayObj) {
		if (variable == null || displayObj == null) {
			logger_.error("getConfigObj",  "ParamError: Params are null.");
			return;
		}

		var toolParam = findIdInConfigTools(displayObj.id);
		var toolType =  findTypeInConfigTools(displayObj.id);

		return createConfigObj(variable, toolType, toolParam);
	}

	// ##############################################
	//  get Default Tools
	// ##############################################

	this.getSettingsFromDefaultTools = function(variableList) {
		var defaultTools = configSettings_.defaultTools;
		var settings = [];
		var i = 0;
		for (i=0; i<defaultTools.length; i++) {
			var variable = variableList[defaultTools[i].valueName];
			if (variable == null) continue;
			var toolIds = defaultTools[i].toolids;
			var configObjs = [];
			var j = 0;
			for (j=0; j<toolIds.length; j++) {
				var toolType = findTypeInConfigTools(toolIds[j]);
				var toolParam = findIdInConfigTools(toolIds[j]);
				var configObj = createConfigObj(variable, toolType, toolParam);
				if (configObj != null) {
					configObjs.push(configObj);
				}
			}
			settings[variable.valueName] = configObjs;
		}
		return settings;
	}

	// **********************************************
	//  private Functions

	// ##############################################
	//  json read and callback create
	// ##############################################

	function readJSON(urlJSON) { // async!!
		loadJSON(urlJSON, function(configJSON) {
			createJSON(configJSON);
		});
	}

	function createJSON(configJSON) {
		configJSON = JSON.parse(configJSON);
		if (configJSON.type == "TOOLS_CONFIG") {
			configTools_ = configJSON.tools;
		} else if (configJSON.type == "VARIABLE_CONFIG") {
			configVariables_ = configJSON.variables;
		} else if (configJSON.type == "SETTINGS") {
			configSettings_ = configJSON;
		} else if (configJSON.type == "DESIGN") {
			configDesign_ = configJSON;
		}
	}

	function checkInitComplete(callback, counter, intervalId) {
		if (configTools_ != null && configVariables_ != null &&
			configSettings_ != null && configDesign_ != null) {
			if (intervalId != null) {
				clearInterval(intervalId);
			}
			callback("Success");
		} else {
			counter += 1;
			if (counter > 5) {
				logger_.error("checkInitComplete", "InitError: Cannot complete init.");
				if (intervalId != null) {
					clearInterval(intervalId);
				}
				callback("Error");
			} else {
				if (intervalId == null) {
					intervalId = setInterval(function() { 
					 	checkInitComplete(callback, counter, intervalId); 
					}, 1000);
				}
			}
		}
	}

	// ##############################################
	//  find Functions
	// ##############################################

	function findTypeInConfigTools(id) {
		var itTool;
		var length = configTools_.length; 
		for (itTool=0; itTool<length; itTool++) {
			var itParam;
			var paramLength = configTools_[itTool].params.length;
			for (itParam=0; itParam<paramLength; itParam++) {
				if (configTools_[itTool].params[itParam].id == id) {
					return configTools_[itTool].toolType;
				}
			}
		}
	}

	function findIdInConfigTools(id) {
		var itTool;
		var length = configTools_.length; 
		for (itTool=0; itTool<length; itTool++) {
			var itParam;
			var paramLength = configTools_[itTool].params.length;
			for (itParam=0; itParam<paramLength; itParam++) {
				if (configTools_[itTool].params[itParam].id == id) {
					return configTools_[itTool].params[itParam];
				}
			}
		}
		return null;
	}

	function findValueNameInConfigVariables(valueName) {
		var itValue;
		var length = configVariables_.length; 
		for (itValue=0; itValue<length; itValue++) {
			if (configVariables_[itValue].valueName == valueName) {
				return configVariables_[itValue];
			}
		}
		return null;
	}

	// ##############################################
	//  helper Functions
	// ##############################################

	function getSize(width, height) {
		if (width >= height) return width;
		else return height;
	}

	function choose(val1, val2) {
		if (val1 != null) return val1;
		else return val2;
	}

	function check(val) {
		if (val != null) return true;
		else return false;
	}

	// ##############################################
	//  create ConfigObj
	// ##############################################

	function createConfigObj(variable, toolType, toolParam) {
		var configObj;
		try {
			switch (toolType) {
				case ToolType.ALTIMETER :
					configObj = new ConfigGaugeObj();
					configObj.toolType = ToolType.ALTIMETER;
					configObj.width = toolParam.width;
					configObj.height = toolParam.height;
					configObj.size = getSize(toolParam.width, toolParam.height);
					break;
				case ToolType.BATTERY :
					configObj = new ConfigGaugeObj();
					configObj.toolType = ToolType.BATTERY;
					configObj.width = toolParam.width;
					configObj.height = toolParam.height;
					configObj.size = getSize(toolParam.width, toolParam.height);
					break;
				case ToolType.CLOCK :
					configObj = new ConfigGaugeObj();
					configObj.toolType = ToolType.CLOCK;
					configObj.width = toolParam.width;
					configObj.height = toolParam.height;
					configObj.size = getSize(toolParam.width, toolParam.height);
					break;
				case ToolType.COMPASS :
					configObj = new ConfigGaugeObj();
					configObj.toolType = ToolType.COMPASS;
					configObj.width = toolParam.width;
					configObj.height = toolParam.height;
					configObj.size = getSize(toolParam.width, toolParam.height);
					configObj.rotateFace = toolParam.rotateFace;
					break;
				case ToolType.DISPLAY_MULTI :
					configObj = new ConfigGaugeObj();
					configObj.toolType = ToolType.DISPLAY_MULTI;
					configObj.width = toolParam.width;
					configObj.height = toolParam.height;
					configObj.unitString = choose(variable.metadata.unitString, toolParam.unitString);
					configObj.unitStringVisible = check(configObj.unitString);
					configObj.headerString = choose( variable.metadata.headerString, toolParam.headerString);
					configObj.headerStringVisible = check(configObj.headerString);
					configObj.detailString = choose(variable.metadata.detailString, toolParam.detailString);
					configObj.detailStringVisible = check(configObj.detailString);
					configObj.linkOldValue = toolParam.linkOldValue;
					configObj.oldValue = toolParam.oldValue;
					break;
				case ToolType.DISPLAY_SINGLE :
					configObj = new ConfigGaugeObj();
					configObj.toolType = ToolType.DISPLAY_SINGLE;
					configObj.width = toolParam.width;
					configObj.height = toolParam.height;
					configObj.autoScroll = toolParam.autoScroll;
					configObj.valuesNumeric = toolParam.valuesNumeric;
					configObj.unitString = choose(variable.metadata.unitString, toolParam.unitString);
					configObj.unitStringVisible = check(configObj.unitString);
					configObj.headerString = choose( variable.metadata.headerString, toolParam.headerString);
					configObj.headerStringVisible = check(configObj.headerString);
					break;
				case ToolType.HORIZON :
					configObj = new ConfigGaugeObj();
					configObj.toolType = ToolType.HORIZON;
					configObj.width = toolParam.width;
					configObj.height = toolParam.height;
					configObj.size = getSize(toolParam.width, toolParam.height);
					break;
				case ToolType.LED :
					configObj = new ConfigGaugeObj();
					configObj.toolType = ToolType.LED;
					configObj.width = toolParam.width;
					configObj.height = toolParam.height;
					configObj.size = getSize(toolParam.width, toolParam.height);
					configObj.blink = toolParam.blink;
					break;
				case ToolType.LEVEL :
					configObj = new ConfigGaugeObj();
					configObj.toolType = ToolType.LEVEL;
					configObj.width = toolParam.width;
					configObj.height = toolParam.height;
					configObj.size = getSize(toolParam.width, toolParam.height);
					configObj.decimalsVisible = toolParam.decimalsVisible;
					break;
				case ToolType.LIGHTBLUB :
					configObj = new ConfigGaugeObj();
					configObj.toolType = ToolType.LIGHTBLUB;
					configObj.width = toolParam.width;
					configObj.height = toolParam.height;
					configObj.lightBlubColor = toolParam.color;
					break;
				case ToolType.LINEAR :
					configObj = new ConfigGaugeObj();
					configObj.toolType = ToolType.LINEAR;
					configObj.width = toolParam.width;
					configObj.height = toolParam.height;
					configObj.minValue = choose(variable.metadata.min, toolParam.min);
					configObj.maxValue = choose(variable.metadata.max, toolParam.max);
					configObj.section = choose(variable.metadata.section, toolParam.section);
					configObj.titleString = choose(variable.metadata.headerString, toolParam.titleString);
					configObj.unitString = choose(variable.metadata.unitString, toolParam.unitString);
					configObj.threshold = choose(variable.metadata.threshold, toolParam.threshold);
					configObj.lcdVisible = toolParam.lcdVisible;
					configObj.gaugeType = toolParam.gaugeType;
					break;
				case ToolType.LINEAR_BARGRAPH :
					configObj = new ConfigGaugeObj();
					configObj.toolType = ToolType.LINEAR_BARGRAPH;
					configObj.width = toolParam.width;
					configObj.height = toolParam.height;
					configObj.minValue = choose(variable.metadata.min, toolParam.min);
					configObj.maxValue = choose(variable.metadata.max, toolParam.max);
					configObj.titleString = choose(variable.metadata.headerString, toolParam.titleString);
					configObj.unitString = choose(variable.metadata.unitString, toolParam.unitString);
					configObj.threshold = choose(variable.metadata.threshold, toolParam.threshold);
					configObj.lcdVisible = toolParam.lcdVisible;
					configObj.valueGradient = choose(variable.metadata.valGrad, toolParam.valueGradient);
					configObj.useValueGradient = check(configObj.valueGradient);
					configObj.section = choose(variable.metadata.section, toolParam.section);
					break;
				case ToolType.ODOMETER :
					configObj = new ConfigGaugeObj();
					configObj.toolType = ToolType.ODOMETER;
					configObj.width = toolParam.width;
					configObj.height = toolParam.height;
					break;
				case ToolType.RADIAL :
					configObj = new ConfigGaugeObj();
					configObj.toolType = ToolType.RADIAL;
					configObj.width = toolParam.width;
					configObj.height = toolParam.height;
					configObj.size = getSize(toolParam.width, toolParam.height);
					configObj.minValue = choose(variable.metadata.min, toolParam.min);
					configObj.maxValue = choose(variable.metadata.max, toolParam.max);
					configObj.gaugeType = toolParam.gaugeType; 
					configObj.titleString = choose(variable.metadata.headerString, toolParam.titleString);
					configObj.unitString = choose(variable.metadata.unitString, toolParam.unitString);
					configObj.threshold = choose(variable.metadata.threshold, toolParam.threshold);
					configObj.lcdVisible = toolParam.lcdVisible;
					configObj.lcdDecimals = toolParam.lcdDecimals;
					configObj.useOdometer = toolParam.useOdometer;
					configObj.odometerParams = toolParam.odometerParams;
					configObj.section = choose(variable.metadata.section, toolParam.section);		
					configObj.area = choose(variable.metadata.area, toolParam.area);
					break;
				case ToolType.RADIAL_BARGRAPH :
					configObj = new ConfigGaugeObj();
					configObj.toolType = ToolType.RADIAL_BARGRAPH;
					configObj.width = toolParam.width;
					configObj.height = toolParam.height;
					configObj.size = getSize(toolParam.width, toolParam.height);
					configObj.minValue = choose(variable.metadata.min, toolParam.min);
					configObj.maxValue = choose(variable.metadata.max, toolParam.max);
	  				configObj.gaugeType = toolParam.gaugeType; 
	  				configObj.titleString = choose(variable.metadata.headerString, toolParam.titleString);
					configObj.unitString = choose(variable.metadata.unitString, toolParam.unitString);
	  				configObj.lcdVisible = toolParam.lcdVisible;
	  				configObj.section = choose(variable.metadata.section, toolParam.section);			
	  				configObj.valueGradient = choose(variable.metadata.valGrad, toolParam.valueGradient);
	  				configObj.useValueGradient = check(configObj.valueGradient);
	  				break;
	  			case ToolType.RADIAL_VERTICAL :
	  				configObj = new ConfigGaugeObj();
	  				configObj.toolType = ToolType.RADIAL_VERTICAL;
	  				configObj.width = toolParam.width;
					configObj.height = toolParam.height;
					configObj.size = getSize(toolParam.width, toolParam.height);
	  				configObj.minValue = choose(variable.metadata.min, toolParam.min);
					configObj.maxValue = choose(variable.metadata.max, toolParam.max);
	  				configObj.titleString = choose(variable.metadata.headerString, toolParam.titleString);
					configObj.unitString = choose(variable.metadata.unitString, toolParam.unitString);
	  				configObj.orientation = toolParam.orientation;
				  	break;
				case ToolType.STOPWATCH :
					configObj = new ConfigGaugeObj();
					configObj.toolType = ToolType.STOPWATCH;
				  	configObj.width = toolParam.width;
					configObj.height = toolParam.height;
					configObj.size = getSize(toolParam.width, toolParam.height);
				  	break;
				case ToolType.TRAFFICLIGHT :
					configObj = new ConfigGaugeObj();
					configObj.toolType = ToolType.TRAFFICLIGHT;
				  	configObj.width = toolParam.width;
				  	configObj.height = toolParam.height;
				  	break;
				case ToolType.WINDDIRECTION :
					configObj = new ConfigGaugeObj();
					configObj.toolType = ToolType.WINDDIRECTION;
				  	configObj.width = toolParam.width;
					configObj.height = toolParam.height;
					configObj.size = getSize(toolParam.width, toolParam.height);
				  	configObj.lcdVisible = toolParam.lcdVisible;
				  	configObj.degreeScaleHalf = toolParam.degreeScaleHalf;
				  	configObj.pointSymbolsVisible = toolParam.pointSymbolsVisible;
				  	break;
				case ToolType.LINE :
					configObj = new ConfigChartObj();
					configObj.toolType = ToolType.LINE;
					configObj.width = toolParam.width;
				  	configObj.height = toolParam.height;			  	
				    configObj.fillColor = toolParam.fillColor;
				    configObj.strokeColor = toolParam.strokeColor;
				    configObj.pointColor = toolParam.pointColor;
				    configObj.pointStrokeColor = toolParam.pointStrokeColor;
				    configObj.animationSteps = toolParam.animationSteps;
	    			configObj.scaleShowGridLines = toolParam.scaleShowGridLines;
				    configObj.amount = toolParam.amount;
				    configObj.trigger = toolParam.trigger;
				    configObj.modulo = toolParam.modulo;
					break;
				case ToolType.TABLE :
					configObj = new ConfigObj();
					configObj.toolType = ToolType.TABLE;
					break;
				default :
				  	logger_.error("createConfigObj", "TypeError: unkown toolType=" + toolType);
			}
		} catch (error) {
			logger_.error("createConfigObj", error);
			return null;
		}

		return configObj;
	}
}
