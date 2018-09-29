#worker
from PyQt5.QtCore import QThread, QObject, pyqtSignal, pyqtSlot
import time
import serial
import serial.tools.list_ports as list_ports


class Worker(QObject):
    finished = pyqtSignal()
    intReady = pyqtSignal([int,int,int,int,int], [str], name="intReady")
    serialport = None
    error = False

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        ports = list(list_ports.comports())
        for p in ports:
            # attempt open ports
            s = serial.Serial(p.device, baudrate=38400,timeout=2)
            data = s.read_until()
            if not data: data = s.read_until()
            if not data: continue
            if data:
                self.serialport = s
                break
        if not self.serialport: self.error = True

    @pyqtSlot()
    def procSerialData(self): # A slot takes no params
        ints = []
        if not self.error:
            while True:
                data = self.serialport.read_until().strip().split(b" ")
                ints = []
                try:
                    ints = [int(x) for x in data[:5]]
                except ValueError:
                    print("Bad data: %s" % data)
                    self.intReady[str].emit("Bad data: %s" % data)
                if len(ints) != 5:
                    print("Bad data: %s" % data)
                    self.intReady[str].emit("Bad data: %s" % data)
                self.intReady.emit(*ints)
        else:
            self.intReady[str].emit("Cannot open serial port.")
        self.finished.emit()
