import time
import random
from umqtt.simple import MQTTClient
from machine import Pin
from uasyncio import sleep, run, CancelledError

# Publish test messages e.g. with:
# mosquitto_pub -t foo_topic -m hello

server = "192.168.3.102"
port_num = 9001
switch_topic = b"home/bedroom/bed-light"
p5 = Pin(5, Pin.OUT)
p5.off()

# Received messages from subscriptions will be delivered to this callback
def sub_cb(topic, msg):
    if msg.decode().lower() == "on":
        p5.on()
    else:
        p5.off()

async def mqtt_task():
    print("main started ...")
    connect = True
    while True:
        try:
            if connect:
                client_id = f"umqtt_client{random.randint(0, 1000)}"
                c = MQTTClient(client_id, server, port=port_num)
                c.set_callback(sub_cb)
                c.connect()
                c.subscribe(switch_topic)
                print("client ", client_id, " connects")
                connect = False
            c.check_msg()
        except Exception as e:
             print("wait message fail, ", e)
             try:
                 c.disconnect()
             except Exception:
                 print("disconnect fail")
             await sleep(1)
             connect = True
        await sleep(0.050)

print("main task start...")
run(mqtt_task())
