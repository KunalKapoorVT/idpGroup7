from PyQt6.QtCore import *; from PyQt6.QtGui import *; from PyQt6.QtWidgets import *; import sys;
from bleak import BleakScanner, BleakClient; import asyncio;

def except_hook(cls, exception, traceback):
    sys.__excepthook__(cls, exception, traceback)

sys.excepthook = except_hook

mac_addr = "94:A9:A8:3A:4D:8B"
uuid = "32ad9d66-a0c5-a791-db00-a16c71b94df4"

tractorCli = BleakClient(mac_addr);


async def scan():
                devices = await BleakScanner.discover()
                return devices


async def connEst():
                
                conn = await tractorCli.connect()
                return conn

class Tractor():
        def __init__(self):
                self.PosX = 0
                self.Posy = 0
                self.AngleZ = 0
                self.VelX = 0
                self.velY = 0
                
                self.prevPosX = 0
                self.prevPosY = 0

                self.moving = False

                self.lastCheckpointX = 0
                self.lastCheckpointY = 0

                self.idealAngleZ = 0

                self.updateCallbacks = []
                
        def update():
                for i in updateCallbacks:
                        i()
                        
        def addUpdateCallback(func):
                updateCallbacks.append(func)

        def removeUpdateCallback(func):
                updateCallbacks.remove(func)


class BTPopoutWindow(QWidget):
        
        def sendTextFromBluetoothPopout(self):
                text = self.popoutBluetoothMessageBox.text()
                print(text)
                pass
        
        def __init__(self):
                super().__init__()

                self.label = QLabel("Bluetooth Console")
                
                self.bluetoothPopoutInfo = QVBoxLayout()
                self.popoutBluetoothInteract = QHBoxLayout()

                self.btMessageHist = QLabel('test message\ntest 2')
                self.btMessageHist.resize(400,200)
                self.btMessageHist.setWordWrap(True)
                self.btMessageHist.setStyleSheet("QLabel { background-color : white; color : blue; }");

                self.popoutBluetoothMessageBox = QLineEdit();
                
                self.popoutSendCustomBluetoothButton = QPushButton("Send")

                self.popoutSendCustomBluetoothButton.clicked.connect(self.sendTextFromBluetoothPopout)
                self.popoutSendCustomBluetoothButton.clicked.connect(self.popoutBluetoothMessageBox.clear)

                self.popoutBluetoothInteract.addWidget(self.popoutBluetoothMessageBox)
                self.popoutBluetoothInteract.addWidget(self.popoutSendCustomBluetoothButton)
                
                self.bluetoothPopoutInfo.addWidget(QLabel("Bluetooth message history: "))
                self.bluetoothPopoutInfo.addWidget(self.btMessageHist)
                self.bluetoothPopoutInfo.addLayout(self.popoutBluetoothInteract)
                
                self.setLayout(self.bluetoothPopoutInfo)

                self.show()


class Window(QWidget):
        
        def startTractor(self):
                asyncio.run(tractorCli.write_gatt_char("0000FFE1-0000-1000-8000-00805f9b34fb",b'Go'))
                pass
                

        def stopTractor(self):
                asyncio.run(tractorCli.write_gatt_char("0000FFE1-0000-1000-8000-00805f9b34fb",b'Stop'))
                pass

        def sendTextFromBluetoothInfo(self):
                text = self.bluetoothMessageBox.text()
                print(text)
                pass

        def popOutBluetoothWindow(self):
                
                self.thisBTPopoutWindow = BTPopoutWindow()
                

        def __init__(self):
                super().__init__()

                self.screenLayout = QHBoxLayout()

                self.sidebar = QVBoxLayout()
                
                self.tractorDisplayScene = QGraphicsScene(0,0,800,600);
                self.tractorDisplayWindow = QGraphicsView(self.tractorDisplayScene)


                self.startButton = QPushButton("Start Tractor")
                self.stopButton = QPushButton("Stop Tractor")

                self.startButton.clicked.connect(self.startTractor);
                self.stopButton.clicked.connect(self.stopTractor);

                self.infoTable = QGridLayout()

                self.infoTable.addWidget(QLabel("PosX: "), 0,0)
                self.infoTable.addWidget(QLabel("PosY: "), 1,0)
                self.infoTable.addWidget(QLabel("AngleZ: "), 2,0)
                self.infoTable.addWidget(QLabel("VelX: "), 3,0)
                self.infoTable.addWidget(QLabel("VelY: "), 4,0)


                self.PosXLabel = QLabel()
                self.PosYLabel = QLabel()
                self.AngleZLabel = QLabel()
                self.VelXLabel = QLabel()
                self.VelYLabel = QLabel()


                self.infoTable.addWidget(self.PosXLabel, 0, 1)
                self.infoTable.addWidget(self.PosYLabel, 1, 1)
                self.infoTable.addWidget(self.AngleZLabel, 2, 1)
                self.infoTable.addWidget(self.VelXLabel, 3, 1)
                self.infoTable.addWidget(self.VelYLabel, 4, 1)


                self.bluetoothInfo = QVBoxLayout()

                self.bluetoothMessageBox = QLineEdit();

                self.bluetoothInteract = QHBoxLayout()
                
                self.sendCustomBluetoothButton = QPushButton("Send")

                self.sendCustomBluetoothButton.clicked.connect(self.sendTextFromBluetoothInfo)
                self.sendCustomBluetoothButton.clicked.connect(self.bluetoothMessageBox.clear)
                
                self.blueToothMenu = QMenu()
                self.blueToothMenu.addAction("Pop out", self.popOutBluetoothWindow)
                
                self.popOutBluetooth = QPushButton();
                self.popOutBluetooth.setMenu(self.blueToothMenu)

                self.bluetoothInteract.addWidget(self.sendCustomBluetoothButton)
                self.bluetoothInteract.addWidget(self.popOutBluetooth)

                self.bluetoothInfo.addWidget(QLabel("Send bluetooth message: "))
                self.bluetoothInfo.addWidget(self.bluetoothMessageBox)
                self.bluetoothInfo.addLayout(self.bluetoothInteract)


                self.sidebar.addWidget(self.startButton)
                self.sidebar.addWidget(self.stopButton)
                self.sidebar.addLayout(self.infoTable)
                self.sidebar.addLayout(self.bluetoothInfo)


                self.screenLayout.addLayout(self.sidebar)
                self.screenLayout.addWidget(self.tractorDisplayWindow)

                self.setLayout(self.screenLayout)

                self.setWindowTitle("IDP Group 7 Tractor Control");

                self.show()
                
##        try:
##            asyncio.run(scan())
##            conn = asyncio.run(connEst())
##        except:
##            print("Could not connect to tractor, oh well")
                
app = QApplication(sys.argv)

window = Window()

app.exec()

asyncio.run(tractorCli.disconnect())

sys.exit()
