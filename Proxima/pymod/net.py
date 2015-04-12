from network import WLAN
w = WLAN(WLAN.STA)
w.connect("ES_5898", security=2, key="neither2")

import demo
demo.btn_stream()
