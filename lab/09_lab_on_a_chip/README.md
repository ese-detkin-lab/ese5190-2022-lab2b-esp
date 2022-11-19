### TODO:

You can now actually measure the timings you modeled in the last lab. Add APDS9960 support to your sensor, then set your system up to capture the following timing info:
- color packet delivered to PIO module
- bit delivered to WS2812 (24x/packet)
- full packet delivered to WS2812
- brightness changed on APDS

Run this experiment in both dark and light room settings (record initial ambient brightness in each case). The Neopixel should start 'off' and the ADPS9960 should be initialized with your preferred sampling rate (you may want to try a few different rates). Run the experiment for at least 100 samples at brightness settings of 0%, 25%, 50%, 75%, 100% (making sure to give the ADPS reading enough time to 'settle' each time Neopixel is turned off).

Report the observed 'jitter' based on misalignment with the free-running PWM module on the WS2012.


------------------------------------------------------------------------

### Workflow
1. Things we reviewed: 
- pico datasheet for PIO part
- analyse our logic analyser's output to understand pio i2c
- Cornell RP2040 course PIO uart, spi examples
- ADPS9960 datasheet
- Traditional I2C read and write protocols and functions

2. Before the lab, we wrote ADPS9960 I2C protocols based on normal I2C hardware to drive the sensor. At first we chose the wrong pin set up due to pico-examples(they used GPIO 4, 5 as SDA, SCL), which resulted in failure to connect the sensor. The correct pin should be 22 and 23.
3. After finished part 6 and 7, we re-programmed the I2C protocol for ADPS 9960 based on PIO. We researched through the i2c.pio and pio_i2c.c and then programmed the driver. 

#### Difficulties 
It's not the hardest part to figure out how ADPS receive and execute command, the datasheet provides clear working flow. However, after we coded the driver, we got data but all the received values are identical. A classmate's github debug log helps a lot https://github.com/Dang0v/ese5190-2022-lab2b-esp/tree/main/lab/09_lab_on_a_chip . There's issue with the pico-example 's source code. 
![image](https://user-images.githubusercontent.com/84453030/202831384-15a5458a-feb0-4079-ab3d-d2e03f48294a.png)
After I modified the repeated-start bit in pio_i2c_read_blocking() and cleared the IRQ at the end of ACK pulse, we successfully got the color and brightness data.

4. When the adps9960 functions, we added the ws2812 'replay' function, by sending the detected color data to ws2812 based on PIO, we see the LED blinks along with the color detected. Just as the firefly before.

![](https://github.com/Thea-E/ese5190-2022-lab2b-esp/blob/main/lab/09_lab_on_a_chip/part09.gif)




