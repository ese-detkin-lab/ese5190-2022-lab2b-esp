### TODO:

Toggle the Qt Py's Neopixel LED when the BOOT button is pressed, using only direct register reads to access the boot button status. You may use the SDK to initialize the board and the pins, and the WS2812 example code to toggle the Neopixel. 


### Registers.c:
https://github.com/Praveen-Raj-u-s/ese5190-2022-lab2b-esp/blob/d4c26d4a6cd7d591de3127db516a7eb6b216701b/lab/01_registers/registers.c


### Result(s):
The inbuilt LED of RP2040 is powered up (glows) for every Boot Button(21) Press.

![Bootsel LED](https://user-images.githubusercontent.com/114270637/208326027-464d19e4-2b52-417d-9fa2-986529ceb65e.gif)
