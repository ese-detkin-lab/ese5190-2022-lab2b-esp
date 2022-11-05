### TODO:

- use your Lab 1 firefly code to generate ADPS9960 I2C traffic and display it on a lab oscilloscope
- take a screenshot of some portion of this exchange, and figure out/annotate what's happening based on the protocol documentation in the ADPS9960 datasheet 

## Lab 2b _ Worked with Amogh and saurabh.

# ESE5190 Lab 2B Part 05 - I2C Traffic


[Link](https://content.arduino.cc/assets/Nano_BLE_Sense_av02-4191en_ds_apds-9960.pdf) to the Datasheet referenced in the document.



![](https://user-images.githubusercontent.com/38959831/200089514-eb981eba-d2c4-43b4-9531-d2a2ae6ddceb.jpeg)
![](https://user-images.githubusercontent.com/38959831/200092742-a24d570e-fd12-4dd4-b66a-e320a5ba2e8f.png)
![](https://user-images.githubusercontent.com/38959831/200092755-4315446a-3480-4ef1-855e-966fb380530d.png)


## I2C Read Cycle
According to the dataset APDS9960 supports only one slave address `0x39`, which we can see it the annotated image. <br />
Here, W=0, which means that it is in Write Mode <br />
Acknowledge bit is `0` <br />
The register address we see here is 0x96, which is the `RDATAL` register, the low byte of Red Channel Data. <br />
Acknowledge bit is `0` <br />
In the next sub-cycle we observe the Slave Address followed by `1` which indicates a Read Cycle<br />
Acknowledge bit is `0` <br />
The data we are sending here is `10110101` followed by `0` Acknowledgement bit, which is then followed by another data byte `00000111`, ending with another `1` as the Acknowledgement bit, indicating that the data has been acknowledged.

So, this is a read cycle in which we are reading the low byte of Red channel data. We see that we are reading `RDATAL`, which is one byte, but we are getting 2 bytes in the read sequence. According to datasheet, Read sequence must read byte pairs, lower byte of register followed by higher byte. So, we see that it reads the lower byte RDATAL `10110101` and higher byte RDATAH `00000111` both. This feature guarantees that the high byte value has not been updated by the ADC between I2C reads.
