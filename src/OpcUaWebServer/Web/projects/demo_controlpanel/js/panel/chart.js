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
//  Chart Builder
// ##############################################

function ChartBuilder() {

    // **********************************************
    //  public

    this.createLine = function(configObj) {
        var cnv_ctx = document.getElementById(configObj.cnv_id).getContext('2d');

        var labelArray = [];
        var dataArray = [];
        var i;
        for (i=0; i<configObj.amount; i++) {
            labelArray.push("");
            dataArray.push(0);
        }

        var lineData = {
            labels: labelArray,
            datasets: [{
                    fillColor: configObj.fillColor,
                    strokeColor: configObj.strokeColor,
                    pointColor: configObj.pointColor,
                    pointStrokeColor: configObj.pointStrokeColor,
                    data: dataArray
        }]};

        var line = new Chart(cnv_ctx).Line(
            lineData, {
                width: configObj.width,
                height: configObj.height,
                animationSteps: configObj.animationSteps,
                bezierCurve: false,
                showTooltips: false,
                pointDot: false,
                pointDotRadius: 0,
                pointHitDetectionRadius: 0,
                scaleShowGridLines : configObj.scaleShowGridLines  
        });

        var toolObj = new ToolChartObj();
        toolObj.type = ToolType.LINE;
        toolObj.set(line, configObj.trigger, configObj.modulo);
        return toolObj;
    }
}