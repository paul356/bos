import time
import random
from umqtt.simple import MQTTClient
from machine import Pin, ADC
from uasyncio import sleep, create_task, run, CancelledError
import sys
import os

# Publish test messages e.g. with:
# mosquitto_pub -t foo_topic -m hello

SERVER = "192.168.3.102"
PORT = 9001
RADIO_RECV = b"home/radio/rcv"
RADIO_PUB  = b"home/radio/txt"
HALF_PEROID = 0.025
THRESHOLD = 2
LONG_FACTOR = 4

LIGHT_VALUE = b"home/bedroom2/light_val"

# switch negative
p5  = Pin(5,  Pin.OUT)
p5.off()
# input side of switch
p18 = Pin(18, Pin.IN, Pin.PULL_UP)
# beep enable cable
p19 = Pin(19, Pin.OUT)
p19.off()
# beep negative
p23 = Pin(23, Pin.OUT)
p23.off()
# light sensor
light_sensor = ADC(Pin(34))
light_sensor.atten(ADC.ATTN_11DB)

async def beep_task(symbol):
    p19.on()
    if symbol == 0:
        await sleep(THRESHOLD * HALF_PEROID)
    else:
        await sleep(LONG_FACTOR * THRESHOLD * HALF_PEROID)
    p19.off()
    await sleep(HALF_PEROID)

# Received messages from subscriptions will be delivered to this callback
def sub_cb(topic, msg):
    print("recieve topic ", msg)
    val = int.from_bytes(msg, sys.byteorder)
    create_task(beep_task(val))

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
            c = MQTTClient(client_id, SERVER, port=PORT)
            c.connect()
            c.set_callback(sub_cb)
            c.subscribe(RADIO_RECV)
            print("client ", client_id, " connects")

            mqtt_conn = c
            retry = False
        except Exception as e:
            print("connect to broker fail, ", e)
            await sleep(1)

async def publish_with_retry(new_value, url, retain=False):
    retry = True
    while retry:
        try:
            print("publish {} to {}", new_value, url.decode())
            mqtt_conn.publish(url, new_value, retain)
            retry = False
        except Exception:
            print("publish new state fail")
            await connect_broker()
            
async def check_mqtt():
    global RADIO_RECV
    global RADIO_PUB

    check_light = False
    
    files = os.listdir("/")
    if "reverse.txt" in files:
        tmp = RADIO_RECV
        RADIO_RECV = RADIO_PUB
        RADIO_PUB = tmp
    if "check_light.txt" in files:
        check_light = True
    
    await connect_broker()

    clicked = 0
    ticks = 0
    while True:
        # Blocking wait for message
        key_state = p18.value()
        if key_state == 0:
            clicked = clicked + 1
        elif key_state == 1:
            if clicked >= LONG_FACTOR * THRESHOLD:
                create_task(publish_with_retry((1).to_bytes(1, sys.byteorder), RADIO_PUB))
            elif clicked >= THRESHOLD:
                create_task(publish_with_retry((0).to_bytes(1, sys.byteorder), RADIO_PUB))
            clicked = 0

        if (ticks % 40) == 0 and check_light:
            light_val = light_sensor.read()
            create_task(publish_with_retry(("%d" % light_val).encode(), LIGHT_VALUE, True))

        try:
            mqtt_conn.check_msg()
        except Exception:
            await connect_broker()
            await sleep(1)
        await sleep(HALF_PEROID)
        ticks = ticks + 1

print("main started ...")
run(check_mqtt())
