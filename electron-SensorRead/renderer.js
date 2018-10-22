// This file is required by the index.html file and will
// be executed in the renderer process for that window.
// All of the Node.js APIs are available in this process.
const { ipcRenderer } = require('electron');

var Highcharts = require('highcharts');
require('highcharts/modules/exporting')(Highcharts);


// Chart settings
settings = {
	plotOptions: {
		gauge : {
		dataLabels: {
			style: { fontSize: "18px"}
		}
	  }
	},
	exporting: { enabled: false },
    chart: {
        type: 'gauge',
        plotBackgroundColor: '#00000000',
        plotBackgroundImage: null,
        plotBorderWidth: 0,
        plotShadow: false,
        animation: false,
        margin: [0, 0, 0, 0]
    },
    title: null,
    pane: {
        startAngle: -178,
        endAngle: 178,
        background: [ ]
    },
    // the value axis
    yAxis: {
        min: 0,
        max: 1025,
        minorTickInterval: 'auto',
        minorTickWidth: 1,
        minorTickLength: 5,
        minorTickPosition: 'inside',
        minorTickColor: '#66666699',
        tickPixelInterval: 30,
        tickWidth: 2,
        tickPosition: 'inside',
        tickLength: 10,
        tickColor: '#66666699',
        labels: {
            step: 2,
            rotation: 'auto'
        },
        title: {
            text: 'Heel',
            style: { fontSize: "24px"}

        },
        plotBands: [{
            from: 420,
            to: 422,
            color: '#55BF3B' // green
        }, {
            from: 165,
            to: 168,
            color: '#DDDF0D' // yellow
        }, {
            from: 888,
            to: 895,
            color: '#DF5353' // red
        }]
    },
    series: [{
        name: 'Speed',
        data: [80],
    }]
}

function update(chart) {
    if (!chart.renderer.forExport) {
        setInterval(function () {
            var point = chart.series[0].points[0],
                newVal,
                inc = Math.round((Math.random() * 1000));

            newVal = inc;

            point.update(newVal);

        }, 550);
    }
}



Highcharts.chart('gauge1', settings, update);
Highcharts.chart('gauge2', settings, update);
Highcharts.chart('gauge3', settings, update);
Highcharts.chart('gauge4', settings, update);
Highcharts.chart('gauge5', settings, update);
