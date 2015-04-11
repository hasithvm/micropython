from network import WLAN
w = WLAN(WLAN.STA)
w.connect("Proxima", security=2, key="fishlure1")

import demo
demo.btn_tweet()
