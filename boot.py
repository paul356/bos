# This file is executed on every boot (including wake-boot from deepsleep)
import webrepl
import esp

esp.osdebug(None)

def do_connect():
    import network

    with open("wifi.txt", "r") as fin:
        ssid = fin.readline().strip()
        password = fin.readline().strip()

    sta_if = network.WLAN(network.STA_IF)
    ap_if = network.WLAN(network.AP_IF)
    if ap_if.active():
        ap_if.active(False)
    if not sta_if.isconnected():
        print('connecting to network...')
        sta_if.active(True)
        sta_if.connect(ssid, password)
        while not sta_if.isconnected():
            pass
    print('Network configuration:', sta_if.ifconfig())

do_connect()
webrepl.start()
