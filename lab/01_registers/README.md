### TODO:

Toggle the Qt Py's Neopixel LED when the BOOT button is pressed, using only direct register reads to access the boot button status. You may use the SDK to initialize the board and the pins, and the WS2812 example code to toggle the Neopixel. 

In this part we should replace the function gpio_get() to direct register reading, the original code can be find in folder code, and we change the code to directly access to registers. 

