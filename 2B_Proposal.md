# Proposal

### Introduction
We're will be using our RP2040 microcontroller/s to design a fun LED and 8x8 Dot matrix reaction mechanism.\
We plan to use an RGB LED, a red LED, a green LED, a yellow LED and a blue LED and a 8x8 MAX7219 dot matrix.\
We will give options to display:
- Alphabets
- Numbers
- Shapes
- Filled Shapes
- Emojis
- Hearts
- Pacman
- Traffic Lights
- RGB
- a default mode (Blue light and a random pattern on 8x8 matrix)

### Principles and Use
- SPI comunication with 8x8 MAX7219 matrix
- GPIO pins for output to LEDs

### Components
- RP2040 microcontroller
- RGB LED
- LED Matrix (8x8)
- Red LED, Green LED, Yellow LED
- Blue LED
- Multiple 47 Ohm and 220 Ohm Resistors
- Breadboard
- Jumper Wires

### Connections
- A0 --> R leg of RGB LED, which is connected to GND through 47 Ohm resistor
- A1 --> G leg of RGB LED, which is connected to GND through 47 Ohm resistor
- A2 --> B leg of RGB LED, which is connected to GND through 47 Ohm resistor
- Ground leg of RGB LED is connected to GND
- A3 --> +ve leg of blue LED, whose -ve leg is connected to GND through 220 Ohm resistor
- SDA --> +ve leg of green LED, whose -ve leg is connected to GND through 220 Ohm resistor (used here as GPIO pin)
- SCL --> +ve leg of yellow LED, whose -ve leg is connected to GND through 220 Ohm resistor (used here as GPIO pin)
- TX --> +ve leg of red LED, whose -ve leg is connected to GND through 220 Ohm resistor (used here as GPIO pin)
- RX --> CS pin of MAX7219 (used here for SPI interface)
- SCK --> CLK pin of MAX7219 (used here for SPI interface)
- MO --> DIN pin of MAX7219 (used here for SPI interface)
- GND --> GND pin of MAX7219
- 5V --> VCC pin of MAX7219

### User interface
- Upon connecting the QT Py to laptop or power source, you will need the serial console to interact
- After opening serial console, press any key
- A menu will appear stating all the display options
- Select any character (a to i) to view the related actions
- When the task is completed, it will display the menu again and await user input.

### Additional
- If the circuit works well, we would like to involve the use of sensors later on.
- Not finalised but could use the APDS-9960 and/or other sensors in the lab

##### Accepting all kinds of weird and fun suggestions (that do not damage the microcontroller and sensors) :D
