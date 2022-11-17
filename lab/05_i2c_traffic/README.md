### TODO:

- use your Lab 1 firefly code to generate ADPS9960 I2C traffic and display it on a lab oscilloscope
- take a screenshot of some portion of this exchange, and figure out/annotate what's happening based on the protocol documentation in the ADPS9960 datasheet 


## Use your Lab 1 firefly code to generate ADPS9960 I2C traffic and display it on a lab oscilloscope
- code
```
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
```
- I2C display on oscilloscope

![scope_1](https://user-images.githubusercontent.com/114196821/200078117-474ad07b-7b00-4249-bbda-25609c8c19f9.png)

## Take a screenshot of some portion of this exchange, and figure out/annotate what's happening based on the protocol documentation in the ADPS9960 datasheet
To realize the interaction between components, the most important part is I2C protocol. I2C protocol uses 2 wires called SCL and SDA to transmit signals between master component (RP2040) and slave component (APDS9960). SCL is serial clock pin and SDA is serial data pin. When the SCL is high potential, SDA will jump from high potential to low potential, that is, the slave will enter to the waiting state to receive the address bit (as shown in figure 1-1). Figure 1-2 clarifies the signal transmission: Part 1 is starting command. If the command of part 1 is issued, the slave’s address in part 2 will be read or written which is controlled by part 3. After the master finishes addressing, it will wait for the slave response ACK signal (part 4). Part 5 is the register address inside the sensor. The oscilloscope shows the similar signal transmission.

<img width="656" alt="lab 2b" src="https://user-images.githubusercontent.com/114196821/200091745-4011c69c-05f8-4d44-a16e-02ff5f218634.PNG">
