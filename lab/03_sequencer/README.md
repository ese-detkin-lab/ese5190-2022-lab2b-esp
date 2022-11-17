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
-----
# commands
| functions |command |
| :--| :--  |
| record  |# \<seconds\>  <frequency(op)>|
| replay | $ \<filename\> <frequency(op)>|
| read address|r \<address(8bits)\>|
| write address|w \<address(8bits)\>\<value>|
<!-- |set I/O pin| p \<gpio\>| -->
|Input gpio|< \<gpio>|
|Output gpio|> \<gpio>|

\* All recorded files are saved under `./records/`<br>
\* `filename` in `./records/1115165839.txt` is just `1115165839`<br>
\* file also records the sampling frequency. Without adjusting \<frequency>, REPL will replay with the recorded frequency.
### Design considerations:
for a easier way reading address via serial, I used `scanf("%08x",&addr)` instead of `getchar_nonblocking`, the drawback is it may introudce blocking time when reading from serial(not sure).

---
## Output demo is under `./code/PCside`

