import time
import random
from umqtt.simple import MQTTClient
from machine import Pin
from uasyncio import sleep, create_task, Loop

# Publish test messages e.g. with:
# mosquitto_pub -t foo_topic -m hello

server = "192.168.3.102"
port_num = 9001
switch_topic = b"home/bedroom/bed-light"
switch_state = None

# Received messages from subscriptions will be delivered to this callback
def sub_cb(topic, msg):
    global switch_state
    switch_state = msg.decode().lower()
    
p18 = Pin(18, Pin.IN, Pin.PULL_UP)

async def check_mqtt():
    global pin_pressed
    global switch_state
    connect = True
    ticks = 0

    clicked = -1
    last_state = 1
    key_state = 1
    while True:
        # Blocking wait for message
        last_state = key_state
        key_state = p18.value()
        if clicked == -1 and key_state == 0 and last_state == 1:
            clicked = ticks + 2
        # debounce key press
        if clicked == ticks and key_state == 0:
            if switch_state == "on":
                c.publish(switch_topic, b"off", retain=True)
                switch_state = "off"
            else:
                c.publish(switch_topic, b"on", retain=True)
                switch_state = "on"
            clicked = -1
        elif clicked == ticks:
            clicked = -1

        try:
            if connect:
                client_id = f"umqtt_client{random.randint(0, 1000)}"
                c = MQTTClient(client_id, server, port=port_num)
                c.connect()
                c.set_callback(sub_cb)
                c.subscribe(switch_topic)
                print("client ", client_id, " connects")
                c.check_msg()
                connect = False
        except Exception as e:
             print("wait message fail, ", e)
             c.disconnect()
             time.sleep(1)
             connect = True

        await sleep(0.005)
        ticks = ticks + 1

print("main started ...")
create_task(check_mqtt())
Loop.run_forever()
