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
//  Gauge Builder
// ##############################################

function GaugeBuilder() {

	// **********************************************
	//  public

	this.createCompass = function(configObj, designObj) {
		var compass = new steelseries.Compass(
			configObj.cnv_id, {
				size : configObj.size,
				rotateFace: configObj.rotateFace
			});
		
		new GaugeDesigner().setDesign(ToolType.COMPASS, compass, designObj);

		var toolObj = new ToolObj();
		toolObj.type = ToolType.COMPASS;
		toolObj.setValue = function(value) {
			if (compass.getValue() != value) {
				compass.setValueAnimated(value);
			}
		}
		return toolObj;
	}

	this.createWindDirection = function(configObj, designObj) {    
		var windDir = new steelseries.WindDirection(
			configObj.cnv_id, {
				size: configObj.size,
				lcdVisible: configObj.lcdVisible,
				degreeScaleHalf: configObj.degreeScaleHalf,
				pointSymbolsVisible: configObj.pointSymbolsVisible
			});

		new GaugeDesigner().setDesign(ToolType.WINDDIRECTION, windDir, designObj);

		var toolObj = new ToolObj();
		toolObj.type = ToolType.WINDDIRECTION;
		toolObj.setValue = function(value) {
			if (windDir.getValueLatest() != value.latest) {
				windDir.setValueAnimatedLatest(value.latest);
			}
			if (windDir.getValueAverage() != value.average) {
        		windDir.setValueAnimatedAverage(value.average);
        	}
		}
		return toolObj;
	}

	this.createLevel = function(configObj, designObj) {
		var level = new steelseries.Level(
			configObj.cnv_id, {
				size: configObj.size,
				decimalsVisible: configObj.decimalsVisible
			});

		new GaugeDesigner().setDesign(ToolType.LEVEL, level, designObj);

		var toolObj = new ToolObj();
		toolObj.type = ToolType.LEVEL;
		toolObj.setValue = function(value) {
			if (level.getValue() != value) {
				level.setValueAnimated(value);
			}
		}
		return toolObj;
	}

	this.createHorizon = function(configObj, designObj) {
		var horizon = new steelseries.Horizon(
			configObj.cnv_id, {
				size: configObj.size
			});

		new GaugeDesigner().setDesign(ToolType.HORIZON, horizon, designObj);

		var toolObj = new ToolObj();
		toolObj.type = ToolType.HORIZON;
		toolObj.setValue = function(value) {
			if (horizon.getPitch() != value.pitch) {
				horizon.setPitch(value.pitch);
			}
			if (horizon.getRoll() != value.roll) {
        		horizon.setRoll(value.roll);
        	}
		}
		return toolObj;
	}

	this.createDisplaySingle = function(configObj, designObj) {
		var single = new steelseries.DisplaySingle(
			configObj.cnv_id, {
				width: configObj.width,
				height: configObj.height,
				autoScroll: configObj.autoScroll,
				valuesNumeric: configObj.valuesNumeric,
				unitString: configObj.unitString,
				unitStringVisible: configObj.unitStringVisible,
				headerString: configObj.headerString,
				headerStringVisible: configObj.headerStringVisible
			});

		new GaugeDesigner().setDesign(ToolType.DISPLAY_SINGLE, single, designObj);

		var toolObj = new ToolObj();
		toolObj.type = ToolType.DISPLAY_SINGLE;
		toolObj.setValue = function(value) {
			single.setValue(value);
		}
		return toolObj;
	}

	this.createDisplayMulti = function(configObj, designObj) {
		var multi = new steelseries.DisplayMulti(
			configObj.cnv_id, {
				width: configObj.width,
				height: configObj.height,
				unitString: configObj.unitString,
				unitStringVisible: configObj.unitStringVisible,
				headerString: configObj.headerString,
				headerStringVisible: configObj.headerStringVisible,
				detailString: configObj.detailString,
				detailStringVisible: configObj.decimalsVisible,
				linkOldValue: configObj.linkOldValue,
				oldValue: configObj.oldValue
			});
	
		new GaugeDesigner().setDesign(ToolType.DISPLAY_MULTI, multi, designObj);

		var toolObj = new ToolObj();
		toolObj.type = ToolType.DISPLAY_MULTI;
		toolObj.setValue = function(value) {
			multi.setValue(value);
		}
		return toolObj;
	}

	this.createLed = function(configObj, designObj) {
		var led = new steelseries.Led(
			configObj.cnv_id, {
				size: 100
			});
		//led.blink(configObj.blink);

		new GaugeDesigner().setDesign(ToolType.LED, led, designObj);

		var toolObj = new ToolObj();
		toolObj.type = ToolType.LED;	
		toolObj.setValue = function(value) {
			led.setLedOnOff(value);
		}
		return toolObj;
	}

	this.createClock = function(configObj, designObj) {
		var clock = new steelseries.Clock(
			configObj.cnv_id, {
				size: configObj.size
			});

		new GaugeDesigner().setDesign(ToolType.CLOCK, clock, designObj);

		var toolObj = new ToolObj();
		toolObj.type = ToolType.CLOCK;
		toolObj.setValue = function(value) {
			if (clock.getValue() != value) {
				clock.setValueAnimated(value);
			}
		}
		return toolObj;
	}

	this.createBattery = function(configObj, designObj) {
		var battery = new steelseries.Battery(
			configObj.cnv_id, {
				size: configObj.size
			});

		new GaugeDesigner().setDesign(ToolType.BATTERY, battery, designObj);

		var toolObj = new ToolObj();
		toolObj.type = ToolType.BATTERY;
		toolObj.setValue = function(value) {
			if (battery.getValue() != value) {
				battery.setValue(value);
			}
		}
		return toolObj;
	}

	this.createAltimeter = function(configObj, designObj) {
		var altimeter = new steelseries.Altimeter(
			configObj.cnv_id, {
				size: configObj.size
			});

		new GaugeDesigner().setDesign(ToolType.ALTIMETER, altimeter, designObj);

		var toolObj = new ToolObj();
		toolObj.type = ToolType.ALTIMETER;
		toolObj.setValue = function(value) {
			if (altimeter.getValue() != value) {
				altimeter.setValueAnimated(value);
			}
		}
		return toolObj;
	}

	this.createOdometer = function(configObj, designObj) {
		var odometer = new steelseries.Odometer(
			configObj.cnv_id, {
                height: configObj.height
			});

		new GaugeDesigner().setDesign(ToolType.ODOMETER, odometer, designObj);

		var toolObj = new ToolObj();
		toolObj.type = ToolType.ODOMETER;
		toolObj.setValue = function(value) {
			if (odometer.getValue() != value) {
				odometer.setValueAnimated(value);
			}
		}
		return toolObj;
	}

	this.createLinear = function(configObj, designObj) {
		var linear = new steelseries.Linear(
			configObj.cnv_id, {	
				width: configObj.width,
				height: configObj.height,
				minValue: configObj.minValue,
				maxValue: configObj.maxValue,
				section: getSection(configObj),
                titleString: configObj.titleString,
                unitString: configObj.unitString,
                threshold: configObj.threshold,
                lcdVisible: configObj.lcdVisible,
                gaugeType: getType(configObj.gaugeType)
            });

		new GaugeDesigner().setDesign(ToolType.LINEAR, linear, designObj);

		var toolObj = new ToolObj();
		toolObj.type = ToolType.LINEAR;
		toolObj.setValue = function(value) {
			if (linear.getValue() != value) {
				linear.setValueAnimated(value);
			}
		}
		return toolObj;
	}

	this.createLinearBargraph = function(configObj, designObj) {
		var linear = new steelseries.LinearBargraph(
			configObj.cnv_id, {	
				width: configObj.width,
				height: configObj.height,
				minValue: configObj.minValue,
				maxValue: configObj.maxValue,
				section: getSection(configObj),
                titleString: configObj.titleString,
                unitString: configObj.unitString,
                threshold: configObj.threshold,
                lcdVisible: configObj.lcdVisible,
                valueGradient: getValGrad(configObj), 
                useValueGradient: configObj.useValueGradient
            });

		new GaugeDesigner().setDesign(ToolType.LINEAR_BARGRAPH, linear, designObj);

		var toolObj = new ToolObj();
		toolObj.type = ToolType.LINEAR_BARGRAPH;
		toolObj.setValue = function(value) {
			if (linear.getValue() != value) {
				linear.setValueAnimated(value);
			}
		}
		return toolObj;
	}        

	this.createRadial = function(configObj, designObj) {
		var radial = new steelseries.Radial(
			configObj.cnv_id, {
				size: configObj.size,
				minValue: configObj.minValue,
				maxValue: configObj.maxValue,
                gaugeType: getType(configObj.gaugeType),
                section: getSection(configObj),
                area: getArea(configObj.area),
                titleString: configObj.titleString,
                unitString: configObj.unitString,
                threshold: configObj.threshold,
                lcdVisible: configObj.lcdVisible,
                lcdDecimals: configObj.lcdDecimals,
                useOdometer: configObj.useOdometer,
                odometerParams: configObj.odometerParams
            });

		new GaugeDesigner().setDesign(ToolType.RADIAL, radial, designObj);

		var toolObj = new ToolObj();
		toolObj.type = ToolType.RADIAL;
		toolObj.setValue = function(value) {
			if (radial.getValue() != value) {
				radial.setValueAnimated(value);
				radial.setOdoValue(value);
			}
		}
		return toolObj;
	}

	this.createRadialBargraph = function(configObj, designObj) {
		var radial = new steelseries.RadialBargraph(
			configObj.cnv_id, {
				size: configObj.size,
				minValue: configObj.minValue,
				maxValue: configObj.maxValue,
                gaugeType: getType(configObj.gaugeType),
                section: getSection(configObj),
                useSectionColors: configObj.useSectionColors,
                valueGradient: getValGrad(configObj),
                useValueGradient: configObj.useValueGradient,
                titleString: configObj.titleString,
                unitString: configObj.unitString,
                lcdVisible: configObj.lcdVisible
            });

		new GaugeDesigner().setDesign(ToolType.RADIAL_BARGRAPH, radial, designObj);

		var toolObj = new ToolObj();
		toolObj.type = ToolType.RADIAL_BARGRAPH;
		toolObj.setValue = function(value) {
			if (radial.getValue() != value) {
				radial.setValueAnimated(value);
			}
		}
		return toolObj;
	}

	this.createRadialVertical = function(configObj, designObj) {
		var vertical = new steelseries.RadialVertical(
			configObj.cnv_id, {
				size: configObj.size,
				minValue: configObj.minValue,
				maxValue: configObj.maxValue,
                titleString: configObj.titleString,
                unitString: configObj.unitString,
                orientation: getOrientation(configObj.orientation)
            });

		new GaugeDesigner().setDesign(ToolType.RADIAL_VERTICAL, vertical, designObj);

		var toolObj = new ToolObj();
		toolObj.type = ToolType.RADIAL_VERTICAL;
		toolObj.setValue = function(value) {
			if (vertical.getValue() != value) {
				vertical.setValueAnimated(value);
			}
		}
		return toolObj;
	}

	this.createStopWatch = function(configObj, designObj) {
		var watch = new steelseries.StopWatch(
            configObj.cnv_id, { 
            	size: configObj.size       
            });

		new GaugeDesigner().setDesign(ToolType.STOPWATCH, watch, designObj);

		var toolObj = new ToolObj();
		toolObj.type = ToolType.STOPWATCH;
		toolObj.setValue = function(value) {
			switch(value) {
				case "STOP"  : watch.stop(); break;
				case "START" : watch.start(); break;
				case "RESET" : watch.reset(); break;
				case "LAP"   : watch.lap(); break;
			}
		}
		return toolObj;
	}

	this.createLightBlub = function(configObj) {
		var lightBulb = new steelseries.LightBulb(
			configObj.cnv_id, {	
				width: configObj.width/2,
				height: configObj.height/2								  
			});
		lightBulb.setGlowColor(configObj.lightBlubColor);

		var toolObj = new ToolObj();
		toolObj.type = ToolType.LIGHTBLUB;
		toolObj.setValue = function(value) {
			if (lightBulb.isOn() != value) {
				lightBulb.setOn(value);
			}
			// if (lightBulb.isOn() != value.on) {
			// 	lightBulb.setOn(value.on);
			// }
			// if (lightBulb.getAlpha() != value.alpha) {
			// 	lightBulb.setAlpha(value.alpha / 100);
			// }
		}
		return toolObj;
	}

	this.createTrafficLight = function(configObj) {
		var trafficLight = steelseries.TrafficLight(
            configObj.cnv_id, {
            	width: configObj.width,
				height: configObj.height
            });

		var toolObj = new ToolObj();
		toolObj.type = ToolType.TRAFFICLIGHT;
		toolObj.setValue = function(value) {
			switch(value.status) {
				case "RED"    : trafficLight.setRedOn(value.on); break;
				case "YELLOW" : trafficLight.setYellowOn(value.on); break;
				case "GREEN"  : trafficLight.setGreenOn(value.on); break;
			}
		}
		return toolObj;
	}

	// **********************************************
	//  private

	function getType(type) {
		switch (type) {
	  		case "TYPE1" : 
	  			return steelseries.GaugeType.TYPE1; 
	  		case "TYPE2" : 
	  			return steelseries.GaugeType.TYPE2; 
	  		case "TYPE3" : 
	  			return steelseries.GaugeType.TYPE3; 
	  		case "TYPE4" : 
	  		    return steelseries.GaugeType.TYPE4; 
	  		default :
	  			return steelseries.GaugeType.TYPE1;
	  	}
	}

	function getOrientation(direction) {
		switch (direction) {
	  		case "NORTH" : 
	  			return steelseries.Orientation.NORTH;
	  		case "SOUTH" : 
	  			return steelseries.Orientation.SOUTH;
	  		case "EAST" : 
	  			return steelseries.Orientation.EAST;
	  		case "WEST" : 
	  			return steelseries.Orientation.WEST;
	  		default :
	  			return steelseries.Orientation.NORTH;
	  	}
	}

	function getSection(configObj) {
		if (configObj.section == null || configObj.section.length != 3) {
			return null;
		}

		var section = [];

		// Low
		section.push(steelseries.Section(
			configObj.section[0].min, 
			configObj.section[0].max, 
			configObj.section[0].color));
		// Medium
		section.push(steelseries.Section(
			configObj.section[1].min, 
			configObj.section[1].max, 
			configObj.section[1].color));
		// High
		section.push(steelseries.Section(
			configObj.section[2].min, 
			configObj.section[2].max, 
			configObj.section[2].color));

		return section;
	}

	function getValGrad(configObj) {
		if (configObj.valGrad == null) {
			return null;
		} else {
			return new steelseries.gradientWrapper(
				configObj.min, configObj.max,
	            [0, 0.25, 0.75, 1],
	            [typeToColor(configObj.valGrad.low), 
	             typeToColor(configObj.valGrad.medium), 
	             typeToColor(configObj.valGrad.normal),
	             typeToColor(configObj.valGrad.high)
	            ]
	        );
		}
	}

	function getArea(buildArea) {	
		if (buildArea == null || buildArea.length != 3) {
			return null;
		} else {			
			var areas = [
				steelseries.Section(
					buildArea[0], 
					buildArea[1], 
					buildArea[2]
				)
			];			
			return areas;
		}
	}

	function typeToColor(type) {
		var color;
		switch (type) {
			case "GREEN":
				color = new steelseries.rgbaColor(0,220,0,0.3);
				break;
			case "RED" :
				color = new steelseries.rgbaColor(220,0,0,0.3);
				break;
			case "BLUE" :
				color = new steelseries.rgbaColor(0,0,220,0.3);
				break;
			case "YELLOW" :
				color = new steelseries.rgbaColor(220,220,0,0.3);
				break;
			case "TEAL" :
				color = new steelseries.rgbaColor(0,220,220,0.3);
				break;
			default:
				color = new setYellowOn.rgbaColor(0, 0, 0, 0);
		}
		return color;
	}
}

// ##############################################
//  Gauge Designer
// ##############################################

function GaugeDesigner() {

	// **********************************************
	//  public

	this.setDesign = function(type, gauge, designObj) { 
		switch(type) {
			case ToolType.COMPASS :
				setFrameDesign(designObj.frameDesign, gauge);
				setBackgroundColor(designObj.backgroundColor, gauge);
				setForegroundType(designObj.foregroundType, gauge);
				setPointerColor(designObj.pointerColor, gauge);
				setPointerType(designObj.pointerType, gauge);
				break;
			case ToolType.LINEAR :
			case ToolType.LINEAR_BARGRAPH :
				setFrameDesign(designObj.frameDesign, gauge);
				setBackgroundColor(designObj.backgroundColor, gauge);
				setValueColor(designObj.valueColor, gauge);
				setLcdColor(designObj.lcdColor, gauge);
				setLedColor(designObj.ledColor, gauge);
				break;
			case ToolType.RADIAL :
			case ToolType.RADIAL_BARGRAPH :
			case ToolType.RADIAL_VERTICAL :
				setFrameDesign(designObj.frameDesign, gauge);
				setBackgroundColor(designObj.backgroundColor, gauge);
				setForegroundType(designObj.foregroundType, gauge);
				setPointerColor(designObj.pointerColor, gauge);
				setPointerType(designObj.pointerType, gauge);
				setLcdColor(designObj.lcdColor, gauge);
				setLedColor(designObj.ledColor, gauge);
				break;
			case ToolType.WINDDIRECTION :
				setFrameDesign(designObj.frameDesign, gauge);
				setBackgroundColor(designObj.backgroundColor, gauge);
				setForegroundType(designObj.foregroundType, gauge);
				setPointerColor(designObj.pointerColor, gauge);
				setPointerType(designObj.pointerType, gauge);
				setLcdColor(designObj.lcdColor, gauge);
				setPointerAvg(designObj.pointerAvg, gauge); 
				setPointerColorAvg(designObj.pointerColorAvg, gauge);
				break;
			case ToolType.STOPWATCH :
				setFrameDesign(designObj.frameDesign, gauge);
				setBackgroundColor(designObj.backgroundColor, gauge);
				setForegroundType(designObj.foregroundType, gauge);
				setPointerColor(designObj.pointerColor, gauge);
				break;
			case ToolType.LEVEL :
				setFrameDesign(designObj.frameDesign, gauge);
				setBackgroundColor(designObj.backgroundColor, gauge);
				setForegroundType(designObj.foregroundType, gauge);
				setPointerColor(designObj.pointerColor, gauge);
				break;
			case ToolType.HORIZON :
				setFrameDesign(designObj.frameDesign, gauge);
				setForegroundType(designObj.foregroundType, gauge);
				break;
			case ToolType.DISPLAY_MULTI :
			case ToolType.DISPLAY_SINGLE :
				setLcdColor(designObj.lcdColor, gauge);
				break;
			case ToolType.LED :
				setLedColor(designObj.ledColor, gauge); 
				break;
			case ToolType.CLOCK :
				setFrameDesign(designObj.frameDesign, gauge);
				setBackgroundColor(designObj.backgroundColor, gauge);
				setForegroundType(designObj.foregroundType, gauge);
				setPointerColor(designObj.pointerColor, gauge);
				setPointerType(designObj.pointerType, gauge);
				break;
			case ToolType.ALTIMETER :
				setFrameDesign(designObj.frameDesign, gauge);
				setBackgroundColor(designObj.backgroundColor, gauge);
				setForegroundType(designObj.foregroundType, gauge);
				setLcdColor(designObj.lcdColor, gauge);
				break;
			case ToolType.BATTERY :
			case ToolType.ODOMETER :
				// nothing
				break;
		} 
	}

	// private

	function setFrameDesign(type, gauge) {
		try {
	        switch(type) {
	            case "BLACK_METAL":
	                gauge.setFrameDesign(steelseries.FrameDesign.BLACK_METAL);
	                break;
	            case "METAL":
	                gauge.setFrameDesign(steelseries.FrameDesign.METAL);
	                break;
	            case "SHINY_METAL":
	                gauge.setFrameDesign(steelseries.FrameDesign.SHINY_METAL);
	                break;
	            case "BRASS":
	                gauge.setFrameDesign(steelseries.FrameDesign.BRASS);
	                break;
	            case "STEEL":
	                gauge.setFrameDesign(steelseries.FrameDesign.STEEL);
	                break;
	            case "CHROME":
	                gauge.setFrameDesign(steelseries.FrameDesign.CHROME);
	                break;
	            case "GOLD":
	                gauge.setFrameDesign(steelseries.FrameDesign.GOLD);
	                break;
	            case "ANTHRACITE":
	                gauge.setFrameDesign(steelseries.FrameDesign.ANTHRACITE);
	                break;
	            case "TILTED_GRAY":
	                gauge.setFrameDesign(steelseries.FrameDesign.TILTED_GRAY);
	                break;
	            case "TILTED_BLACK":
	                gauge.setFrameDesign(steelseries.FrameDesign.TILTED_BLACK);
	                break;
	            case "GLOSSY_METAL":
	                gauge.setFrameDesign(steelseries.FrameDesign.GLOSSY_METAL);
	                break;
	        }
        } catch (err) {} // do nothing 
    }

    function setBackgroundColor(type, gauge) {
    	try {
	        switch(type) {
	            case "DARK_GRAY":
	                gauge.setBackgroundColor(steelseries.BackgroundColor.DARK_GRAY);
	                break;
	            case "SATIN_GRAY":
	                gauge.setBackgroundColor(steelseries.BackgroundColor.SATIN_GRAY);
	                break;
	            case "LIGHT_GRAY":
	                gauge.setBackgroundColor(steelseries.BackgroundColor.LIGHT_GRAY);
	                break;
	            case "WHITE":
	                gauge.setBackgroundColor(steelseries.BackgroundColor.WHITE);
	                break;
	            case "BLACK":
	                gauge.setBackgroundColor(steelseries.BackgroundColor.BLACK);
	                break;
	            case "BEIGE":
	                gauge.setBackgroundColor(steelseries.BackgroundColor.BEIGE);
	                break;
	            case "BROWN":
	                gauge.setBackgroundColor(steelseries.BackgroundColor.BROWN);
	                break;
	            case "RED":
	                gauge.setBackgroundColor(steelseries.BackgroundColor.RED);
	                break;
	            case "GREEN":
	                gauge.setBackgroundColor(steelseries.BackgroundColor.GREEN);
	                break;
	            case "BLUE":
	                gauge.setBackgroundColor(steelseries.BackgroundColor.BLUE);
	                break;
	            case "ANTHRACITE":
	                gauge.setBackgroundColor(steelseries.BackgroundColor.ANTHRACITE);
	                break;
	            case "MUD":
	                gauge.setBackgroundColor(steelseries.BackgroundColor.MUD);
	                break;
	            case "PUNCHED_SHEET":
	                gauge.setBackgroundColor(steelseries.BackgroundColor.PUNCHED_SHEET);
	                break;
	            case "CARBON":
	                gauge.setBackgroundColor(steelseries.BackgroundColor.CARBON);
	                break;
	            case "STAINLESS":
	                gauge.setBackgroundColor(steelseries.BackgroundColor.STAINLESS);
	                break;
	            case "BRUSHED_METAL":
	                gauge.setBackgroundColor(steelseries.BackgroundColor.BRUSHED_METAL);
	                break;
	            case "BRUSHED_STAINLESS":
	                gauge.setBackgroundColor(steelseries.BackgroundColor.BRUSHED_STAINLESS);
	                break;
	            case "TURNED":
	                gauge.setBackgroundColor(steelseries.BackgroundColor.TURNED);
	                break;
	        }
        } catch (err) {} // do nothing
    }

    function setForegroundType(type, gauge) {
    	try {
	        switch(type) {
	            case "TYPE1":
	                gauge.setForegroundType(steelseries.ForegroundType.TYPE1);
	                break;
	            case "TYPE2":
	                gauge.setForegroundType(steelseries.ForegroundType.TYPE2);
	                break;
	            case "TYPE3":
	                gauge.setForegroundType(steelseries.ForegroundType.TYPE3);
	                break;
	            case "TYPE4":
	                gauge.setForegroundType(steelseries.ForegroundType.TYPE4);
	                break;
	            case "TYPE5":
	                gauge.setForegroundType(steelseries.ForegroundType.TYPE5);
	                break;
	        }
        } catch (err) {} // do nothing
    }

    function setPointerColor(type, gauge) {
    	try {
	        switch(type) {
	            case "RED":
	                gauge.setPointerColor(steelseries.ColorDef.RED);
	                break;
	            case "GREEN":
	                gauge.setPointerColor(steelseries.ColorDef.GREEN);
	                break;
	            case "BLUE":
	                gauge.setPointerColor(steelseries.ColorDef.BLUE);
	                break;
	            case "ORANGE":
	                gauge.setPointerColor(steelseries.ColorDef.ORANGE);
	                break;
	            case "YELLOW":
	                gauge.setPointerColor(steelseries.ColorDef.YELLOW);
	                break;
	            case "CYAN":
	                gauge.setPointerColor(steelseries.ColorDef.CYAN);
	                break;
	            case "MAGENTA":
	                gauge.setPointerColor(steelseries.ColorDef.MAGENTA);
	                break;
	            case "WHITE":
	                gauge.setPointerColor(steelseries.ColorDef.WHITE);
	                break;
	            case "GRAY":
	                gauge.setPointerColor(steelseries.ColorDef.GRAY);
	                break;
	            case "BLACK":
	                gauge.setPointerColor(steelseries.ColorDef.BLACK);
	                break;
	            case "RAITH":
	                gauge.setPointerColor(steelseries.ColorDef.RAITH);
	                break;
	            case "GREEN_LCD":
	                gauge.setPointerColor(steelseries.ColorDef.GREEN_LCD);
	                break;
	            case "JUG_GREEN":
	                gauge.setPointerColor(steelseries.ColorDef.JUG_GREEN);
	                break;
	        }
		} catch (err) {} // do nothing
    }

    function setPointerType(type, gauge) {
    	try {
	        switch(type) {
	            case "TYPE1":
	                gauge.setPointerType(steelseries.PointerType.TYPE1);
	                break;
	            case "TYPE2":
	                gauge.setPointerType(steelseries.PointerType.TYPE2);
	                break;
	            case "TYPE3":
	                gauge.setPointerType(steelseries.PointerType.TYPE3);
	                break;
	            case "TYPE4":
	                gauge.setPointerType(steelseries.PointerType.TYPE4);
	                break;
	            case "TYPE5":
	                gauge.setPointerType(steelseries.PointerType.TYPE5);
	                break;
	            case "TYPE6":
	                gauge.setPointerType(steelseries.PointerType.TYPE6);
	                break;
	            case "TYPE7":
	                gauge.setPointerType(steelseries.PointerType.TYPE7);
	                break;
	            case "TYPE8":
	                gauge.setPointerType(steelseries.PointerType.TYPE8);
	                break;
	            case "TYPE9":
	                gauge.setPointerType(steelseries.PointerType.TYPE9);
	                break;
	            case "TYPE10":
	                gauge.setPointerType(steelseries.PointerType.TYPE10);
	                break;
	            case "TYPE11":
	                gauge.setPointerType(steelseries.PointerType.TYPE11);
	                break;
	            case "TYPE12":
	                gauge.setPointerType(steelseries.PointerType.TYPE12);
	                break;
	            case "TYPE13":
	                gauge.setPointerType(steelseries.PointerType.TYPE13);
	                break;
	            case "TYPE14":
	                gauge.setPointerType(steelseries.PointerType.TYPE14);
	                break;
	            case "TYPE15":
	                gauge.setPointerType(steelseries.PointerType.TYPE15);
	                break;
	            case "TYPE16":
	                gauge.setPointerType(steelseries.PointerType.TYPE16);
	                break;
	        }
        } catch (err) {}  // do nothing
    }

    function setLcdColor(type, gauge) {
    	try {
	        switch(type) {
	            case "BEIGE":
	                gauge.setLcdColor(steelseries.LcdColor.BEIGE);
	                break;
	            case "BLUE":
	                gauge.setLcdColor(steelseries.LcdColor.BLUE);
	                break;
	            case "ORANGE":
	                gauge.setLcdColor(steelseries.LcdColor.ORANGE);
	                break;
	            case "RED":
	                gauge.setLcdColor(steelseries.LcdColor.RED);
	                break;
	            case "YELLOW":
	                gauge.setLcdColor(steelseries.LcdColor.YELLOW);
	                break;
	            case "WHITE":
	                gauge.setLcdColor(steelseries.LcdColor.WHITE);
	                break;
	            case "GRAY":
	                gauge.setLcdColor(steelseries.LcdColor.GRAY);
	                break;
	            case "BLACK":
	                gauge.setLcdColor(steelseries.LcdColor.BLACK);
	                break;
	            case "GREEN":
	                gauge.setLcdColor(steelseries.LcdColor.GREEN);
	                break;
	            case "BLUE2":
	                gauge.setLcdColor(steelseries.LcdColor.BLUE2);
	                break;
	            case "BLUE_BLACK":
	                gauge.setLcdColor(steelseries.LcdColor.BLUE_BLACK);
	                break;
	            case "BLUE_DARKBLUE":
	                gauge.setLcdColor(steelseries.LcdColor.BLUE_DARKBLUE);
	                break;
	            case "BLUE_GRAY":
	                gauge.setLcdColor(steelseries.LcdColor.BLUE_GRAY);
	                 break;
	            case "STANDARD":
	                gauge.setLcdColor(steelseries.LcdColor.STANDARD);
	                break;
	            case "STANDARD_GREEN":
	                gauge.setLcdColor(steelseries.LcdColor.STANDARD_GREEN);
	                break;
	            case "BLUE_BLUE":
	                gauge.setLcdColor(steelseries.LcdColor.BLUE_BLUE);
	                break;
	            case "RED_DARKRED":
	                gauge.setLcdColor(steelseries.LcdColor.RED_DARKRED);
	                break;
	            case "DARKBLUE":
	                gauge.setLcdColor(steelseries.LcdColor.DARKBLUE);
	                break;
	            case "LILA":
	                gauge.setLcdColor(steelseries.LcdColor.LILA);
	                break;
	            case "BLACKRED":
	                gauge.setLcdColor(steelseries.LcdColor.BLACKRED);
	                break;
	            case "DARKGREEN":
	                gauge.setLcdColor(steelseries.LcdColor.DARKGREEN);
	                break;
	            case "AMBER":
	                gauge.setLcdColor(steelseries.LcdColor.AMBER);
	                break;
	            case "LIGHTBLUE":
	                gauge.setLcdColor(steelseries.LcdColor.LIGHTBLUE);
	                break;
		        case "SECTIONS":
		            gauge.setLcdColor(steelseries.LcdColor.SECTIONS);
		            break;
	        }
        } catch (err) {} // do nothing
    }

    function setLedColor(type, gauge) {
    	try {
	        switch(type) {
	            case "RED_LED":
	                gauge.setLedColor(steelseries.LedColor.RED_LED);
	                break;
	            case "GREEN_LED":
	                gauge.setLedColor(steelseries.LedColor.GREEN_LED);
	                break;
	            case "BLUE_LED":
	                gauge.setLedColor(steelseries.LedColor.BLUE_LED);
	                break;
	            case "ORANGE_LED":
	                gauge.setLedColor(steelseries.LedColor.ORANGE_LED);
	                break;
	            case "YELLOW_LED":
	                gauge.setLedColor(steelseries.LedColor.YELLOW_LED);
	                break;
	            case "CYAN_LED":
	                gauge.setLedColor(steelseries.LedColor.CYAN_LED);
	                break;
	            case "MAGENTA_LED":
	                gauge.setLedColor(steelseries.LedColor.MAGENTA_LED);
	                break;
	        }
        } catch (err) {} // do nothing
    }

    function setPointerAvg(type, gauge) {
    	try {
	        switch(type) {
	            case "TYPE1":
	                gauge.setPointerTypeAverage(steelseries.PointerType.TYPE1);
	                break;
	            case "TYPE2":
	                gauge.setPointerTypeAverage(steelseries.PointerType.TYPE2);
	                break;
	            case "TYPE3":
	                gauge.setPointerTypeAverage(steelseries.PointerType.TYPE3);
	                break;
	            case "TYPE4":
	                gauge.setPointerTypeAverage(steelseries.PointerType.TYPE4);
	                break;
	            case "TYPE5":
	                gauge.setPointerTypeAverage(steelseries.PointerType.TYPE5);
	                break;
	            case "TYPE6":
	                gauge.setPointerTypeAverage(steelseries.PointerType.TYPE6);
	                break;
	            case "TYPE7":
	                gauge.setPointerTypeAverage(steelseries.PointerType.TYPE7);
	                break;
	            case "TYPE8":
	                gauge.setPointerTypeAverage(steelseries.PointerType.TYPE8);
	                break;
	            case "TYPE9":
	                gauge.setPointerTypeAverage(steelseries.PointerType.TYPE9);
	                break;
	            case "TYPE10":
	                gauge.setPointerTypeAverage(steelseries.PointerType.TYPE10);
	                break;
	            case "TYPE11":
	                gauge.setPointerTypeAverage(steelseries.PointerType.TYPE11);
	                break;
	            case "TYPE12":
	                gauge.setPointerTypeAverage(steelseries.PointerType.TYPE12);
	                break;
	            case "TYPE13":
	                gauge.setPointerTypeAverage(steelseries.PointerType.TYPE13);
	                break;
	            case "TYPE14":
	                gauge.setPointerTypeAverage(steelseries.PointerType.TYPE14);
	                break;
	            case "TYPE15":
	                gauge.setPointerTypeAverage(steelseries.PointerType.TYPE15);
	                break;
	            case "TYPE16":
	                gauge.setPointerTypeAverage(steelseries.PointerType.TYPE16);
	                break;
	        }
        } catch (err) {} // do nothing
    }

    function setPointerColorAvg(type, gauge) {
    	try {
    		switch(type) {
	            case "RED":
	                gauge.setPointerColorAverage(steelseries.ColorDef.RED);
	                break;
	            case "GREEN":
	                gauge.setPointerColorAverage(steelseries.ColorDef.GREEN);
	                break;
	            case "BLUE":
	                gauge.setPointerColorAverage(steelseries.ColorDef.BLUE);
	                break;
	            case "ORANGE":
	                gauge.setPointerColorAverage(steelseries.ColorDef.ORANGE);
	                break;
	            case "YELLOW":
	                gauge.setPointerColorAverage(steelseries.ColorDef.YELLOW);
	                break;
	            case "CYAN":
	                gauge.setPointerColorAverage(steelseries.ColorDef.CYAN);
	                break;
	            case "MAGENTA":
	                gauge.setPointerColorAverage(steelseries.ColorDef.MAGENTA);
	                break;
	            case "WHITE":
	                gauge.setPointerColorAverage(steelseries.ColorDef.WHITE);
	                break;
	            case "GRAY":
	                gauge.setPointerColorAverage(steelseries.ColorDef.GRAY);
	                break;
	            case "BLACK":
	                gauge.setPointerColorAverage(steelseries.ColorDef.BLACK);
	                break;
	            case "RAITH":
	                gauge.setPointerColorAverage(steelseries.ColorDef.RAITH);
	                break;
	            case "GREEN_LCD":
	                gauge.setPointerColorAverage(steelseries.ColorDef.GREEN_LCD);
	                break;
	            case "JUG_GREEN":
	                gauge.setPointerColorAverage(steelseries.ColorDef.JUG_GREEN);
	                break;
	        }
        } catch (err) {} // do nothing
    }

    function setValueColor(type, gauge) {
    	try {
	        switch(type) {
	            case "RED":
	                gauge.setValueColor(steelseries.ColorDef.RED);
	                break;
	            case "GREEN":
	                gauge.setValueColor(steelseries.ColorDef.GREEN);
	                break;
	            case "BLUE":
	                gauge.setValueColor(steelseries.ColorDef.BLUE);
	                break;
	            case "ORANGE":
	                gauge.setValueColor(steelseries.ColorDef.ORANGE);
	                break;
	            case "YELLOW":
	                gauge.setValueColor(steelseries.ColorDef.YELLOW);
	                break;
	            case "CYAN":
	                gauge.setValueColor(steelseries.ColorDef.CYAN);
	                break;
	            case "MAGENTA":
	                gauge.setValueColor(steelseries.ColorDef.MAGENTA);
	                break;
	            case "WHITE":
	                gauge.setValueColor(steelseries.ColorDef.WHITE);
	                break;
	            case "GRAY":
	                gauge.setValueColor(steelseries.ColorDef.GRAY);
	                break;
	            case "BLACK":
	                gauge.setValueColor(steelseries.ColorDef.BLACK);
	                break;
	            case "RAITH":
	                gauge.setValueColor(steelseries.ColorDef.RAITH);
	                break;
	            case "GREEN_LCD":
	                gauge.setValueColor(steelseries.ColorDef.GREEN_LCD);
	                break;
	            case "JUG_GREEN":
	                gauge.setValueColor(steelseries.ColorDef.JUG_GREEN);
	                break;
	        }
        } catch (err) {} // do nothing
    }
}
