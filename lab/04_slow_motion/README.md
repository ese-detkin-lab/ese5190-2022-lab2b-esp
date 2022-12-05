### TODO:

Update your sequencer to be able to slow down and speed up recordings/replay. On the input side, the goal is ultimately to be able to handle the full 4 Gbps "firehose" from the PIO logic analyzer example in the SDK manual, which would fill up 256Kb of RAM in only 31 µs at a system clock speed of 125000 MHz if captured 'raw'! On the output side, the goal is to be able to output precisely timed sequences from the PIO at system clock resolution based on a handful of control points.

- update your sequencer to be able to record just the timestamped transitions between input values, and to be able to play data in this format
- give your sequencer the ability to select a range of output rates for both live and recorded input



### Code:
https://github.com/Praveen-Raj-u-s/ese5190-2022-lab2b-esp/blob/d74c167b3ef72998f3f9e811e8d904f9ef939688/lab/04_slow_motion/Slow%20Motion.c
