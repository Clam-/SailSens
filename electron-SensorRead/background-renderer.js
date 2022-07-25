const {ipcRenderer} = require('electron')
const SerialPort  = require('serialport')
const Readline = require('@serialport/parser-readline')

function sleep(ms) {
  return new Promise(resolve => setTimeout(resolve, ms));
}
console.log("hi pls");

function LOG(s) {
  ipcRenderer.send("log", s);
}

var RUN = true;
var rendererID = null;
var GPORT = null;

ipcRenderer.once('quit', (event, arg) => {
  RUN = false;
  rendererID = null;
  if (GPORT !== null) { GPORT.close(); GPORT = null; }
});
ipcRenderer.once('rendererID', (event, arg) => { rendererID = arg; });


async function connect() {
  var ports = await SerialPort.list();
  ipcRenderer.sendTo(rendererID, "logMsg", 'Ports: ' + (ports ? ports.map(e => e.path).join(","): ""));
  for (const iport of ports) {
    ipcRenderer.sendTo(rendererID, "logMsg", 'Trying port: ' + iport.path);
    port = new SerialPort(iport.path, {
      baudRate: 57600,
      autoOpen: false
    });
    try {
      await port.open();
      // port opened... attempt to get some data?
      console.log("port opened:" + iport.path);
      var data = await port.read(10);
      if (data === null) {
        await sleep(1000);
        data = await port.read(10);
      }
      if (data !== null) {
        // successfull connect and get data
        ipcRenderer.sendTo(rendererID, "logMsg", "Connected to: " + iport.path);
        return port;
      }
      ipcRenderer.sendTo(rendererID, "logMsg", "Timeout on: " + iport.path);
      port.close();
    } catch (err) {
      console.log("CAUGHT ERR:" + err);
      ipcRenderer.sendTo(rendererID, "logMsg", err);
    }
  }
  return null;
}

async function aconnect() {
  SerialPort.list().then(
    ports => ports.forEach(console.log),
    err => console.error(err)
  );
}

function processData(chunk) {
  var a = chunk.split(" ");
  //ipcRenderer.sendTo(rendererID, "logMsg", "Length: " + a.length + " - " + a);
  if (a.length == 9) {
    a = a.map(x => parseInt(x,16));
    ipcRenderer.sendTo(rendererID, 'arrayData', a);
  }
}

function thenWrapper() {
  realLoop().then();
}
async function realLoop() {
  if (!RUN) { return; }
    await sleep(1000);
    GPORT = await connect();
    if (GPORT !== null) {
      // handle disconnect/errors
      GPORT.on('close', () => {
        ipcRenderer.sendTo(rendererID, "logMsg", "Serial Port closed.");
        setTimeout(thenWrapper, 500);
      });
      // start readlining
      const parser = GPORT.pipe(new Readline({ delimiter: '\r\n' }))
      parser.on('data', processData)
    } else {
      // call realLoop again later
      ipcRenderer.sendTo(rendererID, "logMsg", "Couldn't connect.");
      setTimeout(thenWrapper, 500);
    }
}

async function testLoop() {
  await sleep(1000);
  while (true) {
    ipcRenderer.sendTo(rendererID, 'arrayData', [Math.round(Math.random()*1000),Math.round(Math.random()*1000),
        Math.round(Math.random()*1000),Math.round(Math.random()*1000),
        Math.round(Math.random()*1000)]);
    await sleep(400);
  }
}
//testLoop();
thenWrapper();
