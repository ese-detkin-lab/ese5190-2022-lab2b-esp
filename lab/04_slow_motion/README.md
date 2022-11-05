### TODO:

Update your sequencer to be able to slow down and speed up recordings/replay. On the input side, the goal is ultimately to be able to handle the full 4 Gbps "firehose" from the PIO logic analyzer example in the SDK manual, which would fill up 256Kb of RAM in only 31 µs at a system clock speed of 125000 MHz if captured 'raw'! On the output side, the goal is to be able to output precisely timed sequences from the PIO at system clock resolution based on a handful of control points.

- update your sequencer to be able to record just the timestamped transitions between input values, and to be able to play data in this format
- give your sequencer the ability to select a range of output rates for both live and recorded input

## Lab 2b _ Worked with Amogh and saurabh.

# ESE5190 Lab 2B Part 04 - Slow Motion



By adding multipliers and divider variables to delay between recording values, we can control the speed. We have also givena  provision so that the user can select the color of LED while playing the recording.

The speeds user can select are Normal playback, 2x faster playback, 4x faster playback, 2x slower playback, 4x slower playback and can select Red, Blue or Green as colors.

Here, we have played the same recording first with red color at normal speed and second time at 4x faster speed in blue color.



https://user-images.githubusercontent.com/38959831/199866050-03213286-c27b-4ce5-8458-7c3c19aeba2b.mp4
