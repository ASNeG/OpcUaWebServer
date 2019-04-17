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
//  Tool-Factory
// ##############################################	

function ToolFactory() {

    // **********************************************
    //  private

    var gaugeBuilder_ = new GaugeBuilder();
    var chartBuilder_ = new ChartBuilder();

    var gaugeDesignObj_ = new DesignObj();

    // **********************************************
    //  public

    // ##############################################
    //  create
    // ##############################################

    this.create = function(configObj) {
        switch(configObj.toolType) {
            // Gauge Types
            case ToolType.ALTIMETER: 
                return gaugeBuilder_.createAltimeter(configObj, gaugeDesignObj_);
            case ToolType.BATTERY: 
                return gaugeBuilder_.createBattery(configObj, gaugeDesignObj_);
            case ToolType.CLOCK: 
                return gaugeBuilder_.createClock(configObj, gaugeDesignObj_);
            case ToolType.COMPASS: 
                return gaugeBuilder_.createCompass(configObj, gaugeDesignObj_);
            case ToolType.DISPLAY_MULTI: 
                return gaugeBuilder_.createDisplayMulti(configObj, gaugeDesignObj_);
            case ToolType.DISPLAY_SINGLE: 
                return gaugeBuilder_.createDisplaySingle(configObj, gaugeDesignObj_);
            case ToolType.HORIZON: 
                return gaugeBuilder_.createHorizon(configObj, gaugeDesignObj_);
            case ToolType.LED: 
                return gaugeBuilder_.createLed(configObj, gaugeDesignObj_);
            case ToolType.LEVEL: 
                return gaugeBuilder_.createLevel(configObj, gaugeDesignObj_);
            case ToolType.LINEAR: 
                return gaugeBuilder_.createLinear(configObj, gaugeDesignObj_);
            case ToolType.LINEAR_BARGRAPH: 
                return gaugeBuilder_.createLinearBargraph(configObj, gaugeDesignObj_);
            case ToolType.ODOMETER: 
                return gaugeBuilder_.createOdometer(configObj, gaugeDesignObj_);
            case ToolType.RADIAL: 
                return gaugeBuilder_.createRadial(configObj, gaugeDesignObj_);
            case ToolType.RADIAL_BARGRAPH: 
                return gaugeBuilder_.createRadialBargraph(configObj, gaugeDesignObj_);
            case ToolType.RADIAL_VERTICAL: 
                return gaugeBuilder_.createRadialVertical(configObj, gaugeDesignObj_);
            case ToolType.STOPWATCH: 
                return gaugeBuilder_.createStopWatch(configObj, gaugeDesignObj_);
            case ToolType.WINDDIRECTION: 
                return gaugeBuilder_.createWindDirection(configObj, gaugeDesignObj_);
            case ToolType.LIGHTBLUB: 
                return gaugeBuilder_.createLightBlub(configObj);
            case ToolType.TRAFFICLIGHT: 
                return gaugeBuilder_.createTrafficLight(configObj);
   
            // Chart Types
            case ToolType.LINE: 
                return chartBuilder_.createLine(configObj);

            default:
                return null;
        }
    }
}
