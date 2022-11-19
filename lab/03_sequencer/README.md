### TODO:

Create a 'sequencer' that allows you to record BOOT button presses and play them on the Neopixel, and also play a sequence of read/write commands. You should be able to:
- record at a least a few seconds of button input to your RP2040 (in RAM)
- replay a recorded sequence on your NeoPixel
- loop a recording
- save a recording to your laptop (the Python Serial library is one way to do this)
- play a recording from your laptop
- record 'macros' (a sequence of console commands) based on keystrokes in your serial console
- hand-edit a list of register read/write commands on your laptop, and play them on the RP2040
- include multiple I/O sources in a recording, and remap among the following:
    - inputs: BOOT button, console commands, register read/write commands
    - outputs: neopixel color, neopixel brightness, data over serial, register read/write commands


--------------------------------------------------------------------------------------------------------------------------------
### Workflow
1. We checked the output in minicom to see the data successfully come out of registers

2. We created a python script using PySerial library to build our "python minicom". Firstly it will ask user if he wants to record(1), replay(2), or quit(0), then it will start the function accordingly.

3. We adjusted the format of printf in C code, made it print in format: timestamp, 0 or 1

4. In python we got this output, and clean it's format.
```
this_row = str(this_row)[2:-5]
this_row = this_row.split(',')
```
5. We used python to generate a .csv file and input it as value and timestamp:
<img width="114" alt="image" src="https://user-images.githubusercontent.com/84453030/202830891-14b98c7b-a878-48a8-87aa-612f28d71d7c.png">

6. In reverse, we can input the values in the csv file back to the sequencer.c, and replay it in LED.

https://github.com/Thea-E/ese5190-2022-lab2b-esp/blob/main/lab/03_sequencer/sequencer.gif


