// This file is required by the index.html file and will
// be executed in the renderer process for that window.
// All of the Node.js APIs are available in this process.
const { ipcRenderer } = require('electron');
const Denque = require("denque");

const LOG = new Denque([""]);

var Highcharts = require('highcharts');
require('./node_modules/highcharts/highcharts-more.js')(Highcharts);

var logbox = document.getElementById("logbox");

// Chart settings
settings = {
	plotOptions: { gauge : {
		dataLabels: { style: { fontSize: "18px"} },
		dial: { baseLength: "50%", baseWidth: 3, radius: "90%" }
	}},
	exporting: { enabled: false },
  chart: {
      type: 'gauge',
      plotBackgroundColor: 'rgba(0, 0, 0, 0)',
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
      minorTickColor: 'rgba(102,102,102,0.6)',
      tickPixelInterval: 30,
      tickWidth: 2,
      tickPosition: 'inside',
      tickLength: 10,
      tickColor: 'rgba(102,102,102,0.6)',
      labels: {
          step: 2,
          rotation: 'auto'
      },
      title: { text: '', style: { fontSize: "24px"} },
      plotBands: [{
          from: 420,
          to: 423,
          color: '#55BF3B' // green
      }, {
          from: 165,
          to: 169,
          color: '#55BF3B' // green
      }, {
          from: 888,
          to: 893,
          color: '#800080' // red
      }, {
          from: 650,
          to: 656,
          color: 'orange' // red
      }]
    },
    series: [{ name: 'Position', data: [0], }]
}

var CHARTS = []
settings.yAxis.title.text = "MainSheet"
CHARTS.push(Highcharts.chart('mainsheet', settings));
settings.yAxis.title.text = "Rudder"
CHARTS.push(Highcharts.chart('rudder', settings));
settings.yAxis.title.text = "Heel"
CHARTS.push(Highcharts.chart('heel', settings));
settings.yAxis.title.text = "Ram 1"
CHARTS.push(Highcharts.chart('ram1', settings));
settings.yAxis.title.text = "Ram 2"
CHARTS.push(Highcharts.chart('ram2', settings));

var MARKERMAP = {}

var COUNT = 0;

ipcRenderer.on('arrayData', (event, arg) => {
	if (COUNT < 2) { COUNT++; return;}
	COUNT = 0;
	for (var x = 0; x<arg.length; x++) {
		CHARTS[x].series[0].points[0].update(arg[x]);
	}
});
ipcRenderer.on('logMsg', (event, arg) => {
	if (LOG.push(arg) > 20) { LOG.shift(); }
	logbox.value = LOG.toArray().join("\n");
	logbox.scrollTop = logbox.scrollHeight;
});
