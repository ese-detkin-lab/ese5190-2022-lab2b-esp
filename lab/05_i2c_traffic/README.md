### TODO:

- use your Lab 1 firefly code to generate ADPS9960 I2C traffic and display it on a lab oscilloscope
- take a screenshot of some portion of this exchange, and figure out/annotate what's happening based on the protocol documentation in the ADPS9960 datasheet 


### My Notes

Also captured the raw data from the logic analyzer. The computers in the lab also have an Excel plugin that lets you import the data directly from the scopes.
Doesn't look like I got a full transaction due to limits of what I could fit on the screen, but I think I at least got the transmission of the device address.

![](part5-i2c-apds_9960-individual-waveforms-annotations.png)

### First section (red arrow)

|Observed bits|Value|7-bit address|Note|
|-------------|-----|-------------|----|
|01110010|0x72|0x39|Device address, write|
|0|ACK|
|10010100|0x94
|0|ACK|

The RP2040 is requesting the value of the color sensor "Clear" channel, which is located at address 0x94. To do this, it needs to write that address to the APDS-9960, which has a 7-bit device address of 0x39. 

### Second section (red arrow)

|Observed bits|Value|7-bit address|Note|
|-------------|-----|-------------|----|
|01110011|0x73|0x39|Device address, read|
|0|ACK|
|01100010|0x62||data from APDS-9960|
|0|ACK|

The RP2040 requests to read a value from the sensor, and the sensor provides the value 0x62.


