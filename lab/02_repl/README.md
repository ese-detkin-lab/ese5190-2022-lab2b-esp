### TODO:

Create a REPL to let you read and write RP2040 registers from a console. You should be able to:
- select any 32-bit address to read/write (even if not a valid RP2020 address)
- read/write any 32-bit value to this address
- read/write using any of the atomic bit-setting aliases and a 32-bit mask

#
The REPL was coded in C using C SDK. The code is in Finite State Machine style including INPUT, READ_MEM, WRITE_MEM, OUTPUT five states. 
A potential using issue is that NOT any address works under the REPL

<image src=".\display.png">

### Potential scanf() using issues
[Pi Pico - scanf not showing what's been typed?](https://forums.raspberrypi.com/viewtopic.php?t=307749)
    
[scnaf() lead to port not found](https://forums.raspberrypi.com/viewtopic.php?t=327270)