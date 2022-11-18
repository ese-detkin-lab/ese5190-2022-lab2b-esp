# Part 6---Pioscope detailed explanation

TODO:

1. modify the PIO/DMA logic analyzer example to record a timestamped trace of an RP2040-ADPS9960 exchange while the BOOT button is 
    pressed
2. with a partner, connect a second Qt Py to the Stemma Qt bus of an ADPS9960 that's attached to a first Qt Py running the Lab 1 
    Python code
3. record a trace of this exchange


Functionalities achieved:

1. Utilizing the logic analyzer to display any GPIO pin's logic levels through redefining the specific pin setting with timestamp 
    included.

2. Replaying the GPIO pin's behavior through logic analyzer waves.

3. The traces with time stamp will only be displayed when 'boot' button is pressed. When 'boot' is not pressed, the system awaits for next press.

4. When displaying the logic analyzer waves, * means that no change has been detected in value, the number follows means the 
    clock cycle it keeps unchanged. Only the first "-" or "_" will be displayed. 
    For example, "-----" will be displayed as: "-****4".
    
5. When running with a second QT Py to the Stemma Qt Bus of ADPS9960, the detailed SDA and SCL Line of I2C are demonstrated. 
    Of course, the lab 1 Python code is used to run the ADPS9960 so that it is running under the I2C protocol.
    
    The link for the gif for 'connect a second Qt Py to the Stemma Qt bus of an ADPS9960 that's attached to a first Qt Py running 
    the Lab 1 Python code record a trace of this exchange' is as follows:

    https://github.com/skyfall88888/ese5190-2022-lab2b-esp-YF/blob/main/2B_part6_pioscope/pioscope_APDS9960_trace.gif
    

