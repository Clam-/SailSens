const {ipcRenderer} = require('electron')
const SerialPort  = require('serialport')
const Readline = require('@serialport/parser-readline')

function sleep(ms) {
  return new Promise(resolve => setTimeout(resolve, ms));
}

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
  var ports = null;
  try { ports = await SerialPort.list(); }
  catch (err) {
    ipcRenderer.sendTo(rendererID, "logMsg", "List Error: " + err);
  }
  ipcRenderer.sendTo(rendererID, "logMsg", 'Ports: ' + ports.map(e => e.comName).join(","));
  for (const iport of ports) {
    ipcRenderer.sendTo(rendererID, "logMsg", 'Trying port: ' + iport.comName);
    port = new SerialPort(iport.comName, {
      baudRate: 38400,
      autoOpen: false
    });
    try {
      await port.open();
      // port opened... attempt to get some data?
      console.log("port opened:" + iport.comName);
      var data = await port.read(10);
      if (data === null) {
        await sleep(200);
        data = await port.read(10);
      }
      if (data !== null) {
        // successfull connect and get data
        ipcRenderer.sendTo(rendererID, "logMsg", "Connected to: " + iport.comName);
        return port;
      }
      ipcRenderer.sendTo(rendererID, "logMsg", "Timeout on: " + iport.comName);
      port.close();
    } catch (err) {
      console.log("CAUGHT ERR:" + err);
      ipcRenderer.sendTo(rendererID, "logMsg", err);
    }
  }
  return null;
}

function processData(chunk) {
  // only 25% chance to send data until I figure out a better way to skip samples.
  if (Math.random() > 0.25) { return; }
  var a = chunk.split(" ");
  if (a.length == 5) {
    a = a.map(Number);
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
