import Proxima
import pyb
import struct

def init():
    Proxima.begin()

    i2c = pyb.I2C(0)
    i2c.init(i2c.MASTER)

    i2c.send("\x6B\x01", 0x68)
    i2c.send("\x6A\x01", 0x68)
    i2c.send("\x1C\x00", 0x68)
    i2c.send("\x1A\x01", 0x68)
    i2c.send("\x19\x00", 0x68)

    Proxima.end()

def getx():
    Proxima.begin()

    i2c = pyb.I2C(0)
    i2c.init(i2c.MASTER)
    val = struct.unpack("!h", i2c.mem_read(2, 0x68, 0x3B))[0]

    Proxima.end()

    return val

def gety():
    Proxima.begin()

    i2c = pyb.I2C(0)
    i2c.init(i2c.MASTER)
    val = struct.unpack("!h", i2c.mem_read(2, 0x68, 0x3D))[0]

    Proxima.end()

    return val

def getz():
    Proxima.begin()

    i2c = pyb.I2C(0)
    i2c.init(i2c.MASTER)
    val = struct.unpack("!h", i2c.mem_read(2, 0x68, 0x3F))[0]

    Proxima.end()

    return val
