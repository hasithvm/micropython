import twitter
import pyb
import time

def btn_tweet():
    pin = pyb.Pin("GPIO17")
    pin.init(0, mode=pin.IN)

    while True:
        while pin.value() == 1:
            pass

        twitter.send_rnd()
        time.sleep(60000)
