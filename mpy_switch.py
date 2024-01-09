import time
import random
from umqtt.simple import MQTTClient
from machine import Pin
from uasyncio import sleep, create_task, run, CancelledError

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

mqtt_conn = None

async def connect_broker():
    global mqtt_conn
    try:
        mqtt_conn.disconnect()
    except Exception:
        print("disconnect fail")
    mqtt_conn = None
    retry = True
    while retry:
        try:
            client_id = f"umqtt_client{random.randint(0, 1000)}"
            c = MQTTClient(client_id, server, port=port_num)
            c.connect()
            c.set_callback(sub_cb)
            c.subscribe(switch_topic)
            print("client ", client_id, " connects")
            c.check_msg()
            mqtt_conn = c
            retry = False
        except Exception as e:
            print("connect to broker fail, ", e)
            await sleep(1)

async def publish_with_retry(new_value):
    retry = True
    while retry:
        try:
            print("publish ", new_value)
            mqtt_conn.publish(switch_topic, new_value, retain=True)
            retry = False
        except Exception:
            print("publish new state fail")
            await connect_broker()

async def check_mqtt():
    global pin_pressed
    global switch_state
    ticks = 0

    await connect_broker()

    clicked = -1
    last_state = 1
    key_state = 1
    while True:
        # Blocking wait for message
        last_state = key_state
        key_state = p18.value()
        if clicked < ticks and key_state == 0 and last_state == 1:
            clicked = ticks + 2

        # debounce key press
        if clicked == ticks and key_state == 0:
            clicked = -1
            if switch_state == "on":
                await publish_with_retry(b"off")
                switch_state = "off"
            else:
                await publish_with_retry(b"on")
                switch_state = "on"
        elif clicked == ticks:
            clicked = -1

        await sleep(0.005)
        ticks = ticks + 1

print("main started ...")
run(check_mqtt())
