# Proposal for ESE 519 Lab 2B—Gesture Sensing


### LED powered by RP2040 with keyboard control


The following link contains the link to the gif file for the Lab2B Preview.



### Outline

We want to build a gesture sensing system utilizing the RP2040 processor and Adafruit APDS9960 sensor.

We plan to connect three LEDs(red, green, blue) via pin A0, A1, A2 on RP2040 as “signal lights”. When we wave the hands in different directions, the corresponding LED would blink for 1000ms.

RP2040 powers and controls the LEDs with three GPIO lines. APDS9960 will be powered by the 3V voltage output pin from the RP2040.

More specifically:

Horizontal motion (Red LED lights when waving hand from left to right or right to left )

Vertical motion (Green LED lights when waving hand from up to down, or down to up)

Depth motion (Blue LED lights when extending arm from chest further)

### Logic Diagram


<img width="391" alt="Screen Shot 2022-10-20 at 11 34 42 PM" src="https://user-images.githubusercontent.com/95589555/197108380-4d2b15bb-4f1b-4bc3-bb65-24847dd02bb9.png">




### Components request

LEDs: Green, Red, Blue
