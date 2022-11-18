Part 7 TODO:
---modify your sequencer to use the PIO as its primary I/O engine, including the ability to R/W any register


1. Utilized PIO as its primary I/O engine.

2. Utilizing the logic analyzer to display any GPIO pin's logic levels through redefining the specific pin setting.

3. Like in the part 6 pioscope, it will only capture the information when 'boot' button is pressed, otherwise it awaits for the next press.

[Extra Credit] 4. Only when a change in value is detected, the PIO will push data to DMA. Otherwise, it will wait for the next change.

[Extra Credit] 4. C code(pio_sequencer.c) outputs strings in the format of: (timestamp, 1/0). It then communicates with Python which takes in the (timestamp, 1/0) and store it into a csv file.


# Example: The csv was initially blank, the pio_sequencer.c was copied into RP1-RP2, then run pio_sequencer.py to run the RP2040. When 'boot' button is pressed, the changes in value will be recorded(in the format of: (value + timestamp)). These were also stored into the csv file. The gif and csv is attached below:
    
** An gif illustrating writing to python then storing into csv file based on the communication between RP2040 and APDS 9960 is attached at the following link:

https://github.com/skyfall88888/ese5190-2022-lab2b-esp-YF/blob/main/2B_part7_pio_sequencer/p7.gif

** The output csv file(which initially was blank) is attached in the following link:

https://github.com/skyfall88888/ese5190-2022-lab2b-esp-YF/blob/main/2B_part7_pio_sequencer/pio_sequencer.csv




