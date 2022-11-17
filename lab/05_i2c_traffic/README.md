### TODO:

- use your Lab 1 firefly code to generate ADPS9960 I2C traffic and display it on a lab oscilloscope
- take a screenshot of some portion of this exchange, and figure out/annotate what's happening based on the protocol documentation in the ADPS9960 datasheet 
# what's happening
A guess here is a read command sent from pico and a response sent from APDS9960. Because the code read at least twice every 200ms so we could see at least two pair of read-response data frame here.
# image:
<img src=".\media\clear_view.jpg" style="zoom:70%"> <br>
<img src=".\media\one_frame.jpg"><br>
