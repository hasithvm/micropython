import twitter
import pyb
import socket
import accel
import struct
import time

def data_stream(port=9999):
    host = ("75.127.4.246", port)
    accel.init()

    while True:
        try:
            sock = socket.socket()
            sock.connect(host)
            data = "a" * 6
            i = 0
            while True:
                struct.packin(data, "hhh", accel.getx(), accel.gety(), accel.getz())
                sock.send(data)
                i += 1
                print(i)
                time.sleep(50)
        except:
            time.sleep(5000)

def btn_tweet():
    accel.init()
    pin = pyb.Pin("GPIO17")
    pin.init(0, mode=pin.IN)

    while True:
        while pin.value() == 1:
            pass

        twitter.send_rnd(accel.getz() < 0)
        time.sleep(60000)
