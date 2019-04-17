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
//  Response Obj
// ##############################################

function ResponseObj() {
	// header
	this.msgType;
	this.clientHandle;
	this.connectionStatusCode;
	// body
	this.value;
	this.valueStatusCode;
	this.sourceTimestamp;
	this.serverTimestamp;
}

// ##############################################
//  Variable Obj
// ##############################################

function VariableObj() {
	// variable
	this.valueName;
	this.type;
	this.isArray;
	this.metadata = new MetaDataObj();
	// display infos
	this.tools = [];
	this.status;
}

function MetaDataObj() { 
	this.displayName;
	this.min;
	this.max;
	this.headerString;
	this.unitString;
	this.detailString;
	this.section;
	this.valGrad;
	this.area;
	this.threshold;
}

function SectionObj() {
	this.min;
	this.max;
	this.color;
}

function ValGradObj() {
	this.low;
	this.medium;
	this.normal;
	this.high;
}

// ##############################################
//  Tool Obj
// ##############################################

function ToolObj() {

	// **********************************************
	//  public

	this.id;
	this.type;
	this.setValue = function(value) {}
}

var ToolChartObj = function() {
    
    // **********************************************
    //  private

    var count_ = -1;
    var line_;
    var timer_;
    var modulo_;
    var nextValue_;
    var intervalId_;

    // ##############################################
    //  Draw Function
    // ##############################################

    function draw() {
        line_.removeData(); // remove first data
        count_ = (count_ + 1) % modulo_;
        var timestamp = "";
        if (count_ == 0) {
            timestamp = new Date().toLocaleTimeString();
        }
        line_.addData([nextValue_], timestamp);
    }

    // **********************************************
    //  public

    // ##############################################
    //  Setter Funtions
    // ##############################################

    this.setValue = function(value) {
        if (intervalId_ == null) {
          intervalId_ = setInterval(function() { 
          		draw();
            }, timer_); 
        }
        nextValue_ = value;
    }

    this.set = function(line, timer, modulo) {
        line_ = line;
        timer_ = timer;
        modulo_ = modulo;
    }

    this.stop = function() {
    	clearInterval(intervalId_);
    }
}
ToolChartObj.prototype = new ToolObj();
ToolChartObj.prototype.constructor = ToolChartObj;

// ##############################################
//  ConfigObj's
// ##############################################

function ConfigObj() {
	this.id;
	this.cnv_id;
	this.toolType;
	this.width;
	this.height;
	this.size;
}

var ConfigGaugeObj = function() {
	this.minValue;
	this.maxValue;
	this.rotateFace;
	this.lcdVisible;
	this.degreeScaleHalf;
	this.pointSymbolsVisible;
	this.decimalsVisible;
	this.headerString;
	this.headerStringVisible;
	this.unitString;
	this.unitStringVisible;
	this.autoScroll;
	this.valuesNumeric;
	this.detailString;
	this.detailStringVisible;
	this.linkOldValue;
	this.oldValue;
	this.blink;
	this.lightBlubColor;
	this.titleString;
    this.threshold;
    this.orientation;
    this.gaugeType;
    this.section;
    this.area;
    this.lcdDecimals;
    this.useOdometer;
    this.odometerParams;
    this.valueGradient; 
    this.useValueGradient;
}
ConfigGaugeObj.prototype = new ConfigObj();
ConfigGaugeObj.prototype.constructor = ConfigGaugeObj;

var ConfigChartObj = function () {
    this.fillColor;
    this.strokeColor;
    this.pointColor;
    this.pointStrokeColor;
    this.animationSteps; 
    this.scaleShowGridLines;
    this.amount;
    this.trigger;
    this.modulo;
}
ConfigChartObj.prototype = new ConfigObj();
ConfigChartObj.prototype.constructor = ConfigChartObj;

// ##############################################
//  Display Obj
// ##############################################

function DisplayObj() {
	this.id;
	this.description;
	this.icon;
}

// ##############################################
//  DesignObj
// ##############################################

function DesignObj() {
	this.frameDesign = "CHROME";
	this.backgroundColor = "WHITE";
	this.foregroundType = "TYPE1";
	this.pointerColor = "RED";
	this.pointerType = "TYPE1";
	this.lcdColor = "WHITE";
	this.ledColor = "RED_LED";
	this.pointerAvg = "TYPE1";
	this.pointerColorAvg = "BLUE";
	this.valueColor = "BLACK";
}

// ##############################################
//  Type Enums
// ##############################################

var ToolType = {
	// Gauge Types
	ALTIMETER: "ALTIMETER",
	BATTERY: "BATTERY",
	CLOCK: "CLOCK",
	COMPASS: "COMPASS",
	DISPLAY_MULTI: "DISPLAY_MULTI",
	DISPLAY_SINGLE: "DISPLAY_SINGLE",
	HORIZON: "HORIZON",
	LED: "LED",
	LEVEL: "LEVEL",
	LIGHTBLUB: "LIGHTBLUB",
	LINEAR: "LINEAR",
	LINEAR_BARGRAPH: "LINEAR_BARGRAPH",
	ODOMETER: "ODOMETER",
	RADIAL: "RADIAL",
	RADIAL_BARGRAPH: "RADIAL_BARGRAPH",
  	RADIAL_VERTICAL: "RADIAL_VERTICAL",
	STOPWATCH: "STOPWATCH",
	TRAFFICLIGHT: "TRAFFICLIGHT",
	WINDDIRECTION: "WINDDIRECTION",
	
	// Chart Types
	LINE: "LINE",

	// Table
	TABLE: "TABLE"
}
