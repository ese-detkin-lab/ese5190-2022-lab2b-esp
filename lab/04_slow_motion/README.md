### TODO:

Update your sequencer to be able to slow down and speed up recordings/replay. On the input side, the goal is ultimately to be able to handle the full 4 Gbps "firehose" from the PIO logic analyzer example in the SDK manual, which would fill up 256Kb of RAM in only 31 µs at a system clock speed of 125000 MHz if captured 'raw'! On the output side, the goal is to be able to output precisely timed sequences from the PIO at system clock resolution based on a handful of control points.

- update your sequencer to be able to record just the timestamped transitions between input values, and to be able to play data in this format
- give your sequencer the ability to select a range of output rates for both live and recorded input


![a](https://github.com/HaoliangYou/ese5190-2022-lab2b-esp/blob/main/lab/04_slow_motion/4.gif)


On the basis of part 03, we can just add the slow_motion function as follows:

def slow_motion(data_arrays):

two_times_data = []

for data in data_arrays:

two_times_data.append(data)

two_times_data.append(data)

return two_times_data

This function will theoretically slow down your input time.
If you want to speed up, you can use the same method to remove some bits.
