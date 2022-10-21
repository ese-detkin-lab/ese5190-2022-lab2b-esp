# Lab2B Proposal

## GIF of my breadboard LED in action

[Here](https://github.com/minghuin/ese5190-2022-lab2-into-the-void-star/blob/main/lab2B_preview/LAB2BPreview.gif) is the link of my breadboard LED in action. 

In the program,  we initialized the GPIO 23 and set it as output direction. Then, we enabled the keyboard input and keep fetching input from the Stdin. 

- If the input character is "1", then turn the LED on and print the corresponding information
- If the input character is "0", then turn the LED off and print the corresponding information.
- Finally, in other cases, there won't be anything shown in the REPL.



## Outline of what I plan to build

In this lab preview, I am planning to build a LED circuit. However, this LED circuit is not only just a LED lighting circuit. I am planning to add the property of slowly changing to this circuit. This circuit is still pretty simple, but it is cool because the circuit can have some other state and its lightness will not be discrete but continuous.



## Components Requested From Detkin

In this micro-project, the components I need to use are listed below:

- a 100uF capacitor
- a STP16NF06 MOSFET
- 2 750 Ohmn resistors
- a lighting LED

The circuit diagram can be found [here](https://github.com/minghuin/ese5190-2022-lab2-into-the-void-star/blob/main/lab2B_preview/Circuit_Diagram.jpg).



## Questions

My lab partner and I tried our design on the breadboard under 2 different conditions. Firstly, we only use the resistor connected in serial with the LED and it can be slowly turned on but cannot be slowly turned off. And then we add the resistor follows the input signal, it can be slowly turned off. However, it cannot be turned on. Finally, we did it with a manual switch and both turning on and turning off can be achieved.  we quite don't understand the results we get under the first two conditions but we thought it might be related to the resistance of the RP2040. That's why we tried to use a MOSFET to control our circuit. Is our guess correct? 

 