### TODO:

Add support for your custom circuit board to your sequencer, and give a demo.

## Components used for protoboard work
In previous labs, we have done a lot which applied I2C. For example,

We used distance sensor to design a game. Computer generates a matrix with all 0 except one 1. As cursor move up, down, left or right in the matrix according to gesture tested by APDS-9960, users can use gesture to move cursor to approach 1. After approaching 1, users move finger twice around APDS-9960, the keyboard will delete 1 and type 0. At the end of the game, all elements in matrix are 0.

![4 4 gif](https://user-images.githubusercontent.com/114196821/192076289-c3b3bcab-0912-4a18-842c-9e16174ceb31.gif)

To realize the interaction between components, the most important part is I2C protocol. I2C protocol uses 2 wires called SCL and SDA to transmit signals between master component (RP2040) and slave component (APDS9960). SCL is serial clock pin and SDA is serial data pin. When the SCL is high potential, SDA will jump from high potential to low potential, that is, the slave will enter to the waiting state to receive the address bit (as shown in figure 1-1). Figure 1-2 clarifies the signal transmission: Part 1 is starting command. If the command of part 1 is issued, the slave’s address in part 2 will be read or written which is controlled by part 3. After the master finishes addressing, it will wait for the slave response ACK signal (part 4). Part 5 is the register address inside the sensor.

<img width="656" alt="lab 2b" src="https://user-images.githubusercontent.com/114196821/200091745-4011c69c-05f8-4d44-a16e-02ff5f218634.PNG">

