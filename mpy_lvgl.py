from ili9XXX import st7735, COLOR_MODE_RGB, COLOR_MODE_BGR, MADCTL_MX, MADCTL_MY, PORTRAIT, LANDSCAPE
import ntptime
import time
from uasyncio import sleep, run
import lvgl as lv
import lv_utils
from machine import Pin

OFFSET = 4
HOUR = 3

lv.init()
 
event_loop = lv_utils.event_loop(asynchronous=True)

beep = Pin(16, Pin.OUT)
beep.off()

# initialize a st7735 lcd
disp = st7735(mosi=13, clk=14, cs=15, dc=12, rst=4, backlight=-1, backlight_on=1,
              width=128, height=160, rot=PORTRAIT, factor=8, asynchronous=True, colormode=COLOR_MODE_RGB)

def read_image(fileName, width, height, colorFormat):
      with open(fileName,'rb') as f:
            png_data = f.read()
            return lv.img_dsc_t({
                  'header' : {"always_zero" : 0, "w" : width, "h" : height, "cf": colorFormat},
                  'data_size': len(png_data[OFFSET:]),
                  'data': png_data[OFFSET:]})

png_dsc = read_image("cute_dog18.bin", 128, 160, lv.img.CF.INDEXED_4BIT)
png2_dsc = read_image("cute_dog19.bin", 128, 160, lv.img.CF.INDEXED_4BIT)

scr = lv.scr_act()
# turn off screen scrollbar
scr.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
img1 = lv.img(scr)
img1.set_src(png_dsc)
img1.set_align(lv.ALIGN.CENTER)

img2 = lv.img(scr)
img2.set_src(png2_dsc)
img2.set_align(lv.ALIGN.CENTER)

ticks = 0
ntpSynced = False

def sync_with_ntp():
      try:
            global ntpSynced
            ntptime.host = "ntp.ntsc.ac.cn"
            ntptime.settime()
            ntpSynced = True
      except OSError:
            ntpSynced = False
            print("fail to fetch ntp")

def display_time(localtime, labelDate, labelTime):
      timeStr = "#ff0000 {:02}:{:02}:{:02}#".format(*localtime[3:6])
      dateStr = "#0000ff {:04}/{:02}/{:02}#".format(*localtime[0:3])
      labelTime.set_text(timeStr)
      labelDate.set_text(dateStr)

async def sound_task():
      beep.on()
      await sleep(1)
      beep.off()

async def period_routine():
      # display time
      UTC_P8 = 8 * 60 * 60

      dateFontStyle = lv.style_t()
      dateFontStyle.init()
      dateFontStyle.set_text_font(lv.font_montserrat_14)
      dateFontStyle.set_bg_color(lv.color_hex(0xffffff))
      dateFontStyle.set_bg_opa(50)
      
      labelDate = lv.label(scr)
      labelDate.set_align(lv.ALIGN.TOP_MID)
      labelDate.set_text("")
      labelDate.set_recolor(True)
      labelDate.add_style(dateFontStyle, lv.PART.MAIN)
      
      timeFontStyle = lv.style_t()
      timeFontStyle.init()
      timeFontStyle.set_text_font(lv.font_dejavu_16_persian_hebrew)
      timeFontStyle.set_bg_color(lv.color_hex(0xffffff))
      timeFontStyle.set_bg_opa(50)
      
      labelTime = lv.label(scr)
      labelTime.set_align(lv.ALIGN.BOTTOM_MID)
      labelTime.set_text("")
      labelTime.set_recolor(True)
      labelTime.add_style(timeFontStyle, lv.PART.MAIN)

      lastHour = 0
      while True:
            global ticks
            ticks = ticks + 1
          
            # peroid task start
            # sync with ntp server
            if not ntpSynced or (ticks % (3600*8) == 0):
                  sync_with_ntp()
                  if ntpSynced:
                        localTime = time.localtime(time.time() + UTC_P8)
                        lastHour = localTime[HOUR]

            # peroid task end
            actual_time = time.localtime(time.time() + UTC_P8)
            display_time(actual_time, labelDate, labelTime)

            # change background every second
            if (ticks % 2) == 0:
                  img1.move_background()
            else:
                  img2.move_background()

            # beep when the hour changes
            if actual_time[HOUR] != lastHour and ntpSynced:
                  lastHour = actual_time[HOUR]
                  if lastHour >= 8 and lastHour <= 23:
                        beep.on()
            else:
                  beep.off()

            await sleep(1)          

# start routine task
run(period_routine())
