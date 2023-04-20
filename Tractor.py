from bleak import BleakClient, BleakScanner
from math import sin, cos
import asyncio
import time

async def scan():
                devices = await BleakScanner.discover()
                return devices

async def connEst(tractorCli):
    
    conn = await tractorCli.connect()
    return conn

class Tractor():
        def __init__(self):
                self.PosX = 0
                self.PosY = 0
                self.AngleZ = 0
                self.VelX = 0
                self.velY = 0

                self.estimatedXVals = []
                self.estimatedYVals = []

                self.prevEstX = 0
                self.prevEstY = 0
                
                self.prevPosX = 0
                self.prevPosY = 0

                self.moving = False
                self.obstacle = False

                self.idealAngleZ = 0

                self.tapes = []
                self.estTapes = []
                self.obstacles = []
                self.estObstacles = []

                self.userConrolEvents = []
                
                #permanent mac address of the tractor
                self.mac_addr = "94:A9:A8:3A:4D:8B"
                self.gatt_char = "0000FFE1-0000-1000-8000-00805f9b34fb"

                self.bluetoothClient = None
                self.startTIme = time.time()

                self.recvLog = ''

        def time(self):

                return time.time()-self.startTime

        async def quit(self):

                await self.bluetoothClient.stop_notify(self.gatt_char)

                await self.bluetoothClient.disconnect()

        async def send(self, msg):

                print(msg)

                await self.bluetoothClient.write_gatt_char(self.gatt_char, msg)

        def handleMsg(self,msg):
            
            print(msg)

            if msg == "Go Recieved":
                self.moving = True
                self.userConrolEvents.append((self.time(), self.moving))
                
            if msg == "Stop Recieved":
                self.moving = False
                self.userConrolEvents.append((self.time(), self.moving))
                
            if msg == "Obstacle":
                self.moving = False
                self.obstacle = True
                self.obstacles.append((self.time(), self.PosX, self.PosY))
                self.estObstacles.append((self.time(), self.prevEstX, self.prevEstY))
                
            if msg == "Obstacle Cleared":
                self.moving = True
                self.obstacle = False
                prevObs = self.obstacles[-1]
                prevEstObs = self.estObstacles[-1]
                self.obstacles[-1] = (prevObs[0],self.time(),prevObs[1],prevObs[2])
                self.estObstacles[-1] = (prevEstObs[0],self.time(),prevEstObs[1],prevEstObs[2])
                
            if msg == "Read Tape":
                self.tapes.append((self.time(), self.PosX, self.PosY))
                self.estTapes.append((self.time(), self.prevEstX, self.prevEstY))

                
            if msg.startswith("MOVING="):
                self.moving = bool(int(msg[len("MOVING="):]))
                self.userConrolEvents.append((self.time(), self.moving))

            if msg.startswith("GOALANGLE="):
                self.idealAngleZ = float(msg[len("GoALANGLE="):])
                
            if msg.startswith("ANGLEZ="):
                self.AngleZ = float(msg[len("ANGLEZ="):])

                if(self.moving):

                    self.estimatedXVals.append((self.time(), self.prevEstX-sin(self.AngleZ)))
                    self.estimatedYVals.append((self.time(), self.prevEstY+cos(self.AngleZ)))
                    
                    self.prevEstX = self.estimatedXVals[-1][1]
                    self.prevEstY = self.estimatedYVals[-1][1]  

            if msg.startswith("PosX="):
                self.prevPosX = self.PosX
                self.PosX = float(msg[len("PosX="):])
                
                
            if msg.startswith("PosY="):
                self.prevPosY = self.PosY
                self.PosY = float(msg[len("PosY="):])

                
            if msg.startswith("VelX="):
                self.VelX = float(msg[len("VelX="):])

                
            if msg.startswith("VelY="):
                self.VelY = float(msg[len("VelY="):])

        def processRecvLog(self):

                if '\r\n' in self.recvLog:
                    msg = self.recvLog[:self.recvLog.index('\r\n')]
                    self.recvLog = self.recvLog[self.recvLog.index('\r\n')+2:]
                    self.handleMsg(msg)

                
        def receiveNotify(self, handle, data):
            
                print(data)
                
                self.recvLog += data.decode()
                
                self.processRecvLog();


        async def connect(self):
            
                self.bluetoothClient = BleakClient(self.mac_addr);
                
                await self.bluetoothClient.connect()

        async def setRecv(self):

              await self.bluetoothClient.start_notify(self.gatt_char, self.receiveNotify)
                

        async def disconnect(self):

                await self.bluetoothClient.disconnect()
