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
//  Parser
// ##############################################	

function Parser() {	
	
	// **********************************************
	//  private

	var logger_ = new Logger("Parser");

	// **********************************************
	//  public

	// ##############################################
	//  build Variables
	// ##############################################

	this.buildVariables = function(variables) {
		var variableObjArry = [];
		var i;
		for (i=0; i<variables.length; i++) {
			try {
				var tmpVariable = variables[i];
				var variableObj = new VariableObj();
				variableObj.valueName = tmpVariable.Variable;
				variableObj.type = tmpVariable.Type;
				variableObj.isArray = stringToBoolean(tmpVariable.IsArray);

				if (tmpVariable.hasOwnProperty("MetaData")) {
					var tmpMetaData = tmpVariable.MetaData;

					if (tmpMetaData.hasOwnProperty("DisplayName")) {
						variableObj.metadata.displayName = tmpMetaData.DisplayName;
					}

					if (tmpMetaData.hasOwnProperty("Limits")) {
						if (tmpMetaData.Limits.hasOwnProperty("Min")) {
							variableObj.metadata.min = valueToInt(tmpMetaData.Limits.Min);
						}

						if (tmpMetaData.Limits.hasOwnProperty("Max")) {
							variableObj.metadata.max = valueToInt(tmpMetaData.Limits.Max);
						}
					}

					if (tmpMetaData.hasOwnProperty("Info")) {
						if (tmpMetaData.Info.hasOwnProperty("Header")) {
							variableObj.metadata.headerString = tmpMetaData.Info.Header;
						}

						if (tmpMetaData.Info.hasOwnProperty("Unit")) {
							variableObj.metadata.unitString = tmpMetaData.Info.Unit;
						}

						if (tmpMetaData.Info.hasOwnProperty("Detail")) {
							variableObj.metadata.detailString = tmpMetaData.Info.Detail;
						}
					}

					variableObj.metadata.section = parseSections(tmpMetaData);
					variableObj.metadata.valGrad = parseValGrad(tmpMetaData);
					variableObj.metadata.area = parseArea(tmpMetaData);
					variableObj.metadata.threshold = parseThreshold(tmpMetaData);	
				}

				variableObjArry[variableObj.valueName] = variableObj;
			} catch(error) {
				logger_.error("buildVariables", error);
			} 
		}

		return variableObjArry;
	}

	// ##############################################
	//  merge tool list into new variable list
	// ##############################################

	this.mergeToolsIntoNewList = function(oldVariableList, newVariableList) {
		if (oldVariableList == null) {
			return newVariableList;
		}

		var key;
		for (key in newVariableList) {			
            if (newVariableList[key].isArray == false) {
                if (oldVariableList[key] != null) {
                    if (oldVariableList[key].tools.length != 0) {                    	
                    	newVariableList[key].status = oldVariableList[key].status;
                        newVariableList[key].tools = oldVariableList[key].tools;
                    }
                } 
            }
        }

        return newVariableList;
	}

	// ##############################################
	//  cast Value to Type
	// ##############################################

	this.cast = function(valueName, value) {
        try {
            switch (valueName) { 
                case "Int16" : case "UInt16" : 
                case "Int32" : case "UInt32" :
                case "Int64" : case "UInt64" :
                case "Byte" : case "Double" :
                case "Float" :
                    // Standart JS only double, highest number is 2^53
                    value = parseInt(value);
                    break;
                case "Boolean" :
                    if (value == "true" || value == "TRUE" || value == 1 ) {
                        value = true;
                    } else {
                        value = false;
                    }
                    break;
            }
            return value;
        } catch(error) {
            logger_.error("cast", error);
            return null;
        }
    }

	// **********************************************
	//  private

	// ##############################################
	//  parse sections
	// ##############################################

	function parseSections(tmpMetaData) {
		if (tmpMetaData.hasOwnProperty("Section")) {
			var sections = [];
			if (tmpMetaData.Section.hasOwnProperty("Low")) {
				var sectionObj = new SectionObj();
				if (tmpMetaData.Section.Low.hasOwnProperty("Min")) {
					sectionObj.min = valueToInt(tmpMetaData.Section.Low.Min);
				}
				if (tmpMetaData.Section.Low.hasOwnProperty("Max")) {
					sectionObj.max = valueToInt(tmpMetaData.Section.Low.Max);
				}
				if (tmpMetaData.Section.Low.hasOwnProperty("Color")) {
					sectionObj.color = typeToColorString(tmpMetaData.Section.Low.Color); 
				}
				if (sectionObj.min == null || sectionObj.max == null || sectionObj.color == null) {
					throw "ParseException: " + variables[i].Variable + " Section undefine parameters.";
				}
				sections.push(sectionObj);
			}

			if (tmpMetaData.Section.hasOwnProperty("Normal")) {
				var sectionObj = new SectionObj();
				if (tmpMetaData.Section.Normal.hasOwnProperty("Min")) {
					sectionObj.min = valueToInt(tmpMetaData.Section.Normal.Min);
				}
				if (tmpMetaData.Section.Normal.hasOwnProperty("Max")) {
					sectionObj.max = valueToInt(tmpMetaData.Section.Normal.Max);
				}
				if (tmpMetaData.Section.Normal.hasOwnProperty("Color")) {
					sectionObj.color = typeToColorString(tmpMetaData.Section.Normal.Color);
				}
				if (sectionObj.min == null || sectionObj.max == null || sectionObj.color == null) {
					throw "ParseException: " + variables[i].Variable + " Section undefine parameters.";
				}
				sections.push(sectionObj);
			}

			if (tmpMetaData.Section.hasOwnProperty("High")) {
				var sectionObj = new SectionObj();
				if (tmpMetaData.Section.High.hasOwnProperty("Min")) {
					sectionObj.min = valueToInt(tmpMetaData.Section.High.Min);
				}
				if (tmpMetaData.Section.High.hasOwnProperty("Max")) {
					sectionObj.max = valueToInt(tmpMetaData.Section.High.Max);
				}
				if (tmpMetaData.Section.High.hasOwnProperty("Color")) {
					sectionObj.color = typeToColorString(tmpMetaData.Section.High.Color);
				}
				if (sectionObj.min == null || sectionObj.max == null || sectionObj.color == null) {
					throw "ParseException: " + variables[i].Variable + " Section undefine parameters.";
				}
				sections.push(sectionObj);
			}

			return sections;
		}

		return null;
	}

	// ##############################################
	//  parse valgrad
	// ##############################################

	function parseValGrad(tmpMetaData) {
		if (tmpMetaData.hasOwnProperty("ValGrad")) {			
			var valGradObj = new ValGradObj();			
			
			if (tmpMetaData.ValGrad.hasOwnProperty("Low")) {
				valGradObj.low = valueToString(tmpMetaData.ValGrad.Low);
			}
			
			if (tmpMetaData.ValGrad.hasOwnProperty("Medium")) {
				valGradObj.medium = valueToString(tmpMetaData.ValGrad.Medium);
			}
			
			if (tmpMetaData.ValGrad.hasOwnProperty("Normal")) {
				valGradObj.normal = valueToString(tmpMetaData.ValGrad.Normal);
			}
			
			if (tmpMetaData.ValGrad.hasOwnProperty("High")) {
				valGradObj.high = valueToString(tmpMetaData.ValGrad.High);
			}
			
			if (valGradObj.low == null || valGradObj.medium == null || 
				valGradObj.normal == null || valGradObj.high == null) {
				throw "ParseException: " + variables[i].Variable + " ValGrad undefine parameters.";
			}

			return valGradObj;
		}

		return null;
	}

	// ##############################################
	//  parse area
	// ##############################################

	function parseArea(tmpMetaData) {
		if (tmpMetaData.hasOwnProperty("Area")) {
			var area = [];
			if (tmpMetaData.Area.hasOwnProperty("Start")) {
				area.push(tmpMetaData.Area.Start);
			}
			if (tmpMetaData.Area.hasOwnProperty("End")) {
				area.push(tmpMetaData.Area.End);
			}
			if (tmpMetaData.Area.hasOwnProperty("Color")) {
				area.push(typeToColorString(tmpMetaData.Area.Color));
			}
			if (area.length == 3) {
				return area;
			}
		}

		return null;
	}

	// ##############################################
	//  parse Threshold
	// ##############################################

	function parseThreshold(tmpMetaData) {
		if (tmpMetaData.hasOwnProperty("Threshold")) {
			return tmpMetaData.Threshold;
		}

		return null;
	}

	// ##############################################
	//  helper Functions
	// ##############################################

	function stringToBoolean(string){
		string = valueToString(string);
		switch(string.toLowerCase()) {
			case "true": case "yes": case "1": return true;
			case "false": case "no": case "0": case null: return false;
			default: return Boolean(string);
		}
	}

	function valueToInt(integer) {
		return parseInt(integer);
	}

	function valueToString(string) {
		return string.replace(/\s/g, "");
	}

	function typeToColorString(type) {
		var color;
		type = valueToString(type);
		switch (type) {
			case "GREEN":
				color = "rgba(0,220,0,0.3)";
				break;
			case "RED" :
				color = "rgba(220,0,0,0.3)";
				break;
			case "BLUE" :
				color = "rgba(0,0,220,0.3)";
				break;
			case "YELLOW" :
				color = "rgba(220,220,0,0.3)";
				break;
			case "TEAL" :
				color = "rgba(0,220,220,0.3)";
				break;
			default :
				logger_.log("typeToColorString", "Unkwon color-type=" + type);
		}
		return color;
	}
}