from PyQt5.QtWidgets import QApplication, QWidget, QGridLayout, QDial, QTextEdit
from PyQt5.Gui import QPalette, QStyleOptionSlider
from PyQt5.QtCore import QThread
import sys
import worker

SLIDER = QStyleOptionSlider()
SLIDER.maximum = 1025
SLIDER.minimum = 0
SLIDER.dialWrapping = True


class Form(QWidget):
    def __init__(self):
       super().__init__()
       # 1 - create Worker and Thread inside the Form
       self.obj = worker.Worker()  # no parent!
       self.thread = QThread()  # no parent!

       # 2 - Connect Worker`s Signals to Form method slots to post data.
       self.obj.intReady.connect(self.processData)
       self.obj.intReady[str].connect(self.processDataString)

       # 3 - Move the Worker object to the Thread object
       self.obj.moveToThread(self.thread)

       # 4 - Connect Worker Signals to the Thread slots
       self.obj.finished.connect(self.thread.quit)

       # 5 - Connect Thread started signal to Worker operational slot method
       self.thread.started.connect(self.obj.procSerialData)

       # * - Thread finished signal will close the app if you want!
       #self.thread.finished.connect(app.exit)

       # 6 - Start the thread
       self.thread.start()

       # 7 - Start the form
       self.initUI()

    def initUI(self):
        grid = QGridLayout()
        self.log = QTextEdit()
        self.heel = QDial()
        self.heel.setDisabled(True)
        self.heelghost = QDial()
        self.heelghost.setDisabled(True)
        self.sheet = QDial()
        self.sheet.setDisabled(True)
        self.sheetghost = QDial()
        self.sheetghost.setDisabled(True)
        self.rudder = QDial()
        self.rudder.setDisabled(True)
        self.rudderghost = QDial()
        self.rudderghost.setDisabled(True)
        self.ram1 = QDial()
        self.ram1.setDisabled(True)
        self.ram1ghost = QDial()
        self.ram1ghost.setDisabled(True)
        self.ram2 = QDial()
        self.ram2.setDisabled(True)
        self.ram2ghost = QDial()
        self.ram2ghost.setDisabled(True)

        self.setLayout(grid)
        grid.addWidget(self.heel,1,0,3,2)
        grid.addWidget(self.sheet,1,2,3,2)
        grid.addWidget(self.rudder,1,4,3,2)
        grid.addWidget(self.ram1,3,1,3,2)
        grid.addWidget(self.ram2,3,3,3,2)
        grid.addWidget(self.log,0,0,2,6)
        grid.addWidget(self.heelghost,1,0,3,2)
        grid.addWidget(self.sheetghost,1,2,3,2)
        grid.addWidget(self.rudderghost,1,4,3,2)
        grid.addWidget(self.ram1ghost,3,1,3,2)
        grid.addWidget(self.ram2ghost,3,3,3,2)

        palette = self.heelghost.palette()
        color = palette.color(QPalette.Window);
        color.setAlpha(130);

        self.move(300, 150)
        self.resize(1920,1080)
        self.setWindowTitle('Sensor Display')
        self.show()

    def processData(self, si, ri, hi):
        self.sheet.setValue(si)
        self.rudder.setValue(ri)
        self.heel.setValue(hi)
        #print(i)
    def processDataString(self, s):
        self.log.append(s)

app = QApplication(sys.argv)
form = Form()
sys.exit(app.exec_())
