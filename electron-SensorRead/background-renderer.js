const {ipcRenderer} = require('electron')
const serialport = require('serialport')

function sleep(ms) {
  return new Promise(resolve => setTimeout(resolve, ms));
}

var run = true;

ipcRenderer.on('quit', (event, arg) => {
  run = false;
})

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
      ipcRenderer.send('gotMessage', 'Error: ' + err);
    } else {
      ipcRenderer.send('gotMessage', 'Ports: ' + ports.join(", "));
      ports.forEach((port) => {
        ipcRenderer.send('gotMessage', 'Trying port: ' + port.comName;
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
            ipcRenderer.send('gotMessage', "Timeout on: "+ port.comName);
          } else { ipcRenderer.send('gotMessage', "Unexpected open false."); }
        } catch (err) {
          ipcRenderer.send('gotMessage', err);
        }
      }
    }
  }
  return null;
}

function processData(chunk) {
  var a = chunk.split(" ");
  if (a.length == 5) {
    ipcRenderer.send('gotData', a);
  }
}

function loop() {
  if (!run) { return; }
  if (!connected) {
    await sleep(500);
    port = connect();
    // start readlining
    const parser = port.pipe(new Readline({ delimiter: '\r\n' }))
    parser.on('data', processData)
  }
}
