import Proxima
import pyb
import struct

_i2c = pyb.I2C(0)

def init():
    Proxima.begin()

    _i2c.init(_i2c.MASTER)

    _i2c.send("\x6B\x01", 0x68)
    _i2c.send("\x6A\x01", 0x68)
    _i2c.send("\x1C\x00", 0x68)
    _i2c.send("\x1A\x01", 0x68)
    _i2c.send("\x19\x00", 0x68)

    Proxima.end()

def getx():
    Proxima.begin()

    _i2c.init(_i2c.MASTER)
    val = struct.unpack("!h", _i2c.mem_read(2, 0x68, 0x3B))[0]

    Proxima.end()

    return val

def gety():
    Proxima.begin()

    _i2c.init(_i2c.MASTER)
    val = struct.unpack("!h", _i2c.mem_read(2, 0x68, 0x3D))[0]

    Proxima.end()

    return val

def getz():
    Proxima.begin()

    _i2c.init(_i2c.MASTER)
    val = struct.unpack("!h", _i2c.mem_read(2, 0x68, 0x3F))[0]

    Proxima.end()

    return val
