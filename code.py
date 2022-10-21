import board
import digitalio
import time
led = digitalio.DigitalInOut(board.SCL1)
led.direction = digitalio.Direction.OUTPUT
while True:
    time.sleep(0.5)
    led.value=True
    time.sleep(0.5)
    led.value=False