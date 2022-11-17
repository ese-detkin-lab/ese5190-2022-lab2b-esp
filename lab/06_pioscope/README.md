### TODO:

- modify the PIO/DMA logic analyzer example to record a timestamped trace of an RP2040-ADPS9960 exchange while the BOOT button is pressed
- with a partner, connect a second Qt Py to the Stemma Qt bus of an ADPS9960 that's attached to a first Qt Py running the Lab 1 Python code
- record a trace of this exchange

plot the serial output of SCL and SDA



The sampling rate is 8 times of i2c rate.
![part6](https://user-images.githubusercontent.com/44985032/202356854-b05a79a1-a0d6-4a14-b36e-455c29cf121d.png)

As we can see, there are 4 data package


![part6](https://user-images.githubusercontent.com/44985032/202356859-482573ae-87a9-410e-9cc7-789ff93ee552.png)

5ms per packet interval, which is 0.005 x 1600 x 8000 = 64000 cycle.

Thus, the PIO scope seems work correctly.
