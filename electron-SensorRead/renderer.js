// This file is required by the index.html file and will
// be executed in the renderer process for that window.
// All of the Node.js APIs are available in this process.
const { ipcRenderer } = require('electron');
const Denque = require("denque");

const LOG = new Denque([""]);

var Highcharts = require('highcharts');
require('./node_modules/highcharts/highcharts-more.js')(Highcharts);

var logbox = document.getElementById("logbox");
var opsval = document.getElementById("opsval");
var opsmin = document.getElementById("opsmin");
var opsmax = document.getElementById("opsmax");
var opsvall = 9999999;
var opsvalh = 0;

var DYNMAP = [document.getElementById("maindynamic").children[0], document.getElementById("rudderdynamic").children[0],
 	document.getElementById("heeldynamic").children[0]]
var STATICMAP = [document.getElementById("mainstatic").children[0], document.getElementById("rudderstatic").children[0],
 	document.getElementById("heelstatic").children[0]]

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

function parity_check(v){
  //http://graphics.stanford.edu/~seander/bithacks.html#ParityNaive
  v ^= v >>> 16;
  v ^= v >>> 8;
  v ^= v >>> 4;
  v &= 0xf;
  return (0x6996 >>> v) & 1;
}

ipcRenderer.on('arrayData', (event, arg) => {
	// ops
	var ops = arg[5];
  if (ops != 0) {
    if (ops<opsvall) {opsvall = ops; opsmin.innerText = opsvall;}
  	if (ops>opsvalh) {opsvalh = ops; opsmax.innerText = opsvalh;}
  	opsval.innerText = ops;
  }
	// encoder and ex data
	for (var x=0; x<3; x++) {
    // encoders
    var t = arg[x] & 0b111110;
    var parity = parity_check(arg[x]>>>0);
    t = t | parity;
		DYNMAP[x].innerText = (t>>>0).toString(2);
    // ex data
    var ta = arg[x+6]; //& 0b111111
		STATICMAP[x].innerText = (ta>>>0).toString(2);
	}

	// only update graphs every second cycle
	if (COUNT < 2) { COUNT++; return;}
	COUNT = 0;
	for (var x = 0; x<5; x++) { // 5 dials
		// shift encoders only
		if (x<3) {CHARTS[x].series[0].points[0].update(arg[x]>>>6);}
		else {CHARTS[x].series[0].points[0].update(arg[x]);}
	}
});
ipcRenderer.on('logMsg', (event, arg) => {
	if (LOG.push(arg) > 20) { LOG.shift(); }
	logbox.value = LOG.toArray().join("\n");
	logbox.scrollTop = logbox.scrollHeight;
});
