import * as log from './logger.js'
import * as util from './util.js'

var RUNNING = false;
export function isRunning() {
  return RUNNING;
}

function sleep(ms) {
  return new Promise(resolve => setTimeout(resolve, ms));
}

class LineBreakTransformer {
  constructor() {
    // A container for holding stream data until a new line.
    this.chunks = "";
  }
  transform(chunk, controller) {
    // Append new chunks to existing chunks.
    this.chunks += chunk;
    // For each line breaks in chunks, send the parsed lines out.
    const lines = this.chunks.split("\r\n");
    this.chunks = lines.pop();
    lines.forEach((line) => controller.enqueue(line));
  }
  flush(controller) {
    // When the stream is closed, flush any remaining chunks out.
    controller.enqueue(this.chunks);
  }
}

class FancyPort {
  constructor(p) {
    this.port = p;
    this.done = false;
  }
  async open() {
    log.log("Opening port: " + this.port);
    //this.port.setSignals({ dataTerminalReady: false }); 256000
    await this.port.open({ baudRate: 56700, flowControl: "hardware" });
    log.log("Opened port: " + this.port);
  }
  async openTest() {
    await this.open();
    var found = false;
    var data = await this.readWithTimeout(2000);
    if (data === null || data === "") { data = await this.readWithTimeout(2000); } // retry
    if (data != null && data != "") {
      found = true;
    }
    await this.port.close();
    return found;
  }
  async openProper() {
    await this.open();
    // setup read
    const textDecoder = new TextDecoderStream();
    this.readableStreamClosed = this.port.readable.pipeTo(textDecoder.writable);
    this.reader = textDecoder.readable
      .pipeThrough(new TransformStream(new LineBreakTransformer()))
      .getReader();
  }
  async read() {
    const { value, done } = await this.reader.read();
    this.done = done;
    return value;
  }
  async readWithTimeout(timeout) {
    const reader = this.port.readable.getReader();
    const timer = setTimeout(() => {
//      reader.releaseLock();  // do this when Chrome implements... for now:
      reader.cancel();
    }, timeout);
    const { value, done } = await reader.read();
    clearTimeout(timer);
    reader.releaseLock();
    return value;
  }
  async close() {
    await this.reader.cancel();
    this.reader.releaseLock();
    await this.readableStreamClosed.catch(() => { /* Ignore the error */ });
    // Allow the serial port to be closed later.
    await this.port.close();
  }
}

async function getPort(isClick) {
  if (isClick) {
    try { fancyp = new FancyPort(await navigator.serial.requestPort());}
    catch (e) { log.log("No port selected"); console.log("No port selected"); return null; }
    if (await fancyp.openTest()) { return fancyp; }
  }
  // attempt on any previous ports
  const ports = await navigator.serial.getPorts();
  var fancyp = null;
  for (const p of ports) {
    fancyp = new FancyPort(p)
    if (await fancyp.openTest()) { return fancyp; }
  }
  return null;
}

var opsval = document.getElementById("opsval");
var opsmin = document.getElementById("opsmin");
var opsmax = document.getElementById("opsmax");
var opsvall = 9999999;
var opsvalh = 0;
var tick = 0;
function processData(a, charts) {
  //arg [main, rudder, heel, ram1, ram2, ops, mainext, rudderext, heelext]
  var ops = a[5];
  if (ops != 0) {
    if (ops<opsvall) {opsvall = ops; opsmin.innerText = opsvall;}
  	if (ops>opsvalh) {opsvalh = ops; opsmax.innerText = opsvalh;}
  	opsval.innerText = ops;
  }
  // encoder and ex data
  for (var x=0; x<3; x++) {
    // encoders
    var parity = util.parity_check(a[x]>>>0);
  }
  for (var x = 0; x<5; x++) { // 5 dials
  	// shift encoders only
  	if (x<3) {charts[x].series[0].points[0].update(a[x]>>>6);}
  	else {charts[x].series[0].points[0].update(a[x]);}
  }
}

export async function datastart(isClick, charts) {
  var fancyport = await getPort(isClick);
  if (fancyport) {
    await fancyport.openProper();
    RUNNING = true;
    while (!fancyport.done) {
      var data = await fancyport.read();
      var a = data.split(" ");
      if (a.length == 9) {
        a = a.map(x => parseInt(x,16));
        processData(a, charts);
      }
    }
    await fancyport.close();
    RUNNING = false;
  }
}
