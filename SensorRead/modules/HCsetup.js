// Chart settings
var settings = {
	plotOptions: { gauge : {
		dataLabels: { style: { fontSize: "18px"} },
		dial: { baseLength: "50%", baseWidth: 3, radius: "100%", borderWidth: 0 }
	}},
	exporting: { enabled: false },
  chart: {
      type: 'gauge',
      plotBackgroundColor: 'rgba(0, 0, 0, 0)',
      plotBackgroundImage: null,
      plotBorderWidth: 0,
      plotShadow: false,
      animation: false,
      margin: [0, 0, 0, 0],
      spacing: [0,0,0,0]
  },
  tooltip: { enabled: false },
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
    },
    series: [{ name: 'Position', data: [0], }]
}
export function setup() {
  var charts = []
  charts.push(Highcharts.chart('mainsheet', Highcharts.merge(settings, {yAxis: {title: {text: "Main Sheet" }}} )));
  charts.push(Highcharts.chart('rudder', Highcharts.merge(settings, {yAxis: {title: {text: "Rudder" }}} )));
  charts.push(Highcharts.chart('heel', Highcharts.merge(settings, {yAxis: {title: {text: "Heel" }}} )));
  charts.push(Highcharts.chart('ram1', Highcharts.merge(settings, {yAxis: {title: {text: "Ram 1" }}} )));
  charts.push(Highcharts.chart('ram2', Highcharts.merge(settings, {yAxis: {title: {text: "Ram 2" }}} )));
  return charts;
}
