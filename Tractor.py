from bleak import BleakClient, BleakScanner
import asyncio
import sys


async def scan():
    devices = await BleakScanner.discover()
    return devices


async def connEst(tractorCli):
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

        # permanent mac address of the tractor
        self.mac_addr = "94:A9:A8:3A:4D:8B"
        self.gatt_char = "0000FFE1-0000-1000-8000-00805f9b34fb"

        self.bluetoothClient = None

    def send(self, msg):
        print(msg)

        x = self.bluetoothClient.write_gatt_char(self.gatt_char, msg)
        asyncio.run(x)

    def receiveNotify(self, sender, data: bytearray):
        sender = self.gatt_char
        print(f"{sender}: {data}")

    ##
    ##        def receive(self):
    ##
    ##                receiver = self.bluetoothClient.read_gatt_char(self.gatt_char)
    ##                data = asyncio.run(receiver)
    ##                print(data)

    def connect(self):
        self.bluetoothClient = BleakClient(self.mac_addr)

        asyncio.run(connEst(self.bluetoothClient))

    def setRecv(self):
        BleakClient(self.mac_addr).bluetoothClient.start_notify('0000ffe1-0000-1000-8000-00805f9b34fb', self.receiveNotify)

        #x = self.bluetoothClient.start_notify('00002a05-0000-1000-8000-00805f9b34fb', self.receiveNotify)
        #asyncio.run(x)

    ##                x = self.bluetoothClient.start_notify('00002a04-0000-1000-8000-00805f9b34fb', self.receiveNotify)
    ##                asyncio.run(x)
    ##
    ##                x = self.bluetoothClient.start_notify('00002902-0000-1000-8000-00805f9b34fb', self.receiveNotify)
    ##                asyncio.run(x)
    ##
    ##                x = self.bluetoothClient.start_notify('00002901-0000-1000-8000-00805f9b34fb', self.receiveNotify)
    ##                asyncio.run(x)

    def disconnect(self):
        asyncio.run(self.bluetoothClient.disconnect())
