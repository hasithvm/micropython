import socket
import accel
import struct
import time

host = ("75.127.4.246", 9997)

def update():
    accel.init()
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
