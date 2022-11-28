import time
import board
from adafruit_apds9960.apds9960 import APDS9960
import neopixel

pixels = neopixel.NeoPixel(board.NEOPIXEL, 1)

i2c = board.STEMMA_I2C()
apds = APDS9960(i2c)
apds.enable_color = True
apds.color_integration_time = 10

while True:
    # wait for color data to be ready
    while not apds.color_data_ready:
        time.sleep(0.005)

    # get the data and print the different channels
    r, g, b, c = apds.color_data
    print("red: ", r)
    print("green: ", g)
    print("blue: ", b)
    print("clear: ", c)

    pixels.fill((0, g/257, 0))
    time.sleep(0.5)