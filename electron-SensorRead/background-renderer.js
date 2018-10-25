const {ipcRenderer} = require('electron')
const serialport = require('serialport')

function sleep(ms) {
  return new Promise(resolve => setTimeout(resolve, ms));
}

var run = true;
var rendererID = null;
var port = null;

ipcRenderer.once('quit', (event, arg) => {
  run = false;
  rendererID = null;
  if (port !== null) { port.close(); port = null; }
});
ipcRenderer.once('rendererID', (event, arg) => { rendererID = arg; });


function openPort(method) {
  return new Promise(function(resolve, reject) {
        return method(function(err) {
            return err ? reject(err) : resolve(true);
        });
    });
}

function connect() {
  serialport.list((err, ports) => {
    if (err) {
      ipcRenderer.sendTo(rendererID, "logMsg", 'Error: ' + err);
    } else {
      ipcRenderer.sendTo(rendererID, "logMsg", 'Ports: ' + ports.join(", "));
      ports.forEach(async (port) => {
        ipcRenderer.sendTo(rendererID, "logMsg", 'Trying port: ' + port.comName);
        port = new SerialPort(port.comName, {
          baudRate: 38400,
          autoOpen: false
        })
        try {
          let result = await openPort(port.open);
          if (result) {
            // port opened... attempt to get some data?
            var data = serialport.read(10);
            if (data === null) { sleep(200); data = serialport.read(10); }
            if (data !== null) { return port; }
            ipcRenderer.sendTo(rendererID, "logMsg", "Timeout on: "+ port.comName);
          } else { ipcRenderer.sendTo(rendererID, "logMsg", "Unexpected open false."); }
        } catch (err) {
          ipcRenderer.sendTo(rendererID, "logMsg", err);
        }
      });
    }
  });
  return null;
}

function processData(chunk) {
  var a = chunk.split(" ");
  if (a.length == 5) {
    a = a.map(Number);
    ipcRenderer.sendTo(rendererID, 'arrayData', a);
  }
}

async function realLoop() {
  if (!run) { return; }
    await sleep(500);
    port = connect();
    if (port !== null) {
      // handle disconnect/errors
      port.on('close', () => {
        ipcRenderer.sendTo(rendererID, "logMsg", "Serial Port closed.");
        setTimeout(realLoop, 500);
      });
      // start readlining
      const parser = port.pipe(new Readline({ delimiter: '\r\n' }))
      parser.on('data', processData)
    } else {
      // call realLoop again later
      ipcRenderer.sendTo(rendererID, "logMsg", "Couldn't connect.");
      setTimeout(realLoop, 500);
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
realLoop();
