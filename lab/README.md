Outline:
- 01_registers
- 02_repl
- 03_sequencer
- 04_slow_motion
- 05_i2c_traffic
- 06_pioscope
- 07_pio_sequencer
- 08_adps_protocol
- 09_lab_on_a_chip
- 10_protoboard
- EC_analog_knob
- EC_ansi_escapes
- EC_asm_registers
- EC_dma_i2c
- EC_hardware_i2c
- EC_interrupt_i2c
- EC_lovr_viz
- EC_pwm_scope
- EC_socket_server
- EC_speaker

## Lab Feedback Questions
As asked on ED/Canvas

I had previously written some thoughts in the README for section 07, and I am also committing my working notes for that section to lay out my thought process.


> Which parts of the lab specification have you found most confusing or difficult to understand? Be specific, and quote any parts of the lab description relevant to your answer (e.g., "modify the PIO/DMA logic analyzer example to record a timestamped trace of an RP2040-ADPS9960 exchange while the BOOT button is pressed.") I will make particular notice of anything that seems underspecified (e.g., words whose meaning is not clear from context, or conflicting interpretations of a deliverable).

- Through part 03, I found the assignment descriptions to be relatively clear. Taken as part of the whole Lab 2B, I can see how they iteratively build the feature set necessary for the final lab sections. In section 04, the first sentence tripped me up when it us to "slow down and speed up recordings/replay" (changing replay time makes sense, but how do you speed up a recording?). The bulleted requirements mostly clear things up and this section was ultimately not difficult to implement. 
- The least clear section is 07, in my opinion. The second half of the sentence, "..including the ability to R/W any register" made me second-guess many of my potential designs, since it implies that reading and writing arbitrary internal registers should be able to be done with the same resolution as the GPIO. Is this the case? [My section 07 writeup](07_pio_sequencer/README.md) reiterates this, but I think more specific requirements would go a long way to guiding the design in the direction that I believe the lab is leading us.
- The [notes](07_pio_sequencer/notes.md) that I've just committed includes a bit at the end regarding my plans for finishing section 07 that I'd like some feedback on, if possible. If this is on the right track, I think some additional clues in the lab instructions or lectures would have helped me arrive there sooner. Perhaps some mention of using the PIO to read/write values to locations other than the GPIO, and/or a component to one of the sections similar to section 01, but for the timer ("get the current absolute time using only register reads"). 


Some questions I still have:
- Do we need to monitor/set all 30 GPIO pins?
- Do we need to capture at the full 125 MHz rate? (mentioned in the extra credit requirements, but is it required for standard credit, or is it a stretch goal?)
- Does the reading/setting of internal registers need to occur at the same rate/resolution as the GPIO R/W (that is, does it need to be handled by the PIO or am I overthinking it)?

> Which lab topics have you found most confusing or difficult to understand? E.g., "serial communication with Python," "aliased bitwise operations," "programming the PIO," etc. Be specific, and describe any lingering areas of confusion and/or anything that has helped you navigate them.
- Surprisingly, considering my progress with the lab, I feel like I conceptually understand all the topics that have been presented. It's the combining them all into a single program that meets requirements (that I am unsure of) that has been challenging. I get what I think the lab is going for, but I feel I ran into one or two issues that held everything up.



> Which parts of the lab have you found most difficult to implement? Again, be specific, citing any parts of the lab materials & your own code that are relevant to your answer.
- Section 06 and 07 have by far given me the most trouble. For both, the challenge is mostly in the implementation of the timestamps/delta encoding. As I mention in [my section 07 writeup](07_pio_sequencer/README.md), my attempts at storing a timestamp for a captured pin transition were fruitless, especially if the requirement is to capture at 125 MHz (which is not stated in the lab instructions, but is mentioned in the extra credit requirements posted just before the original deadline). *Is* this the requirement? I now have a couple ideas for implementing the functionality mentioned in section 07, but I don't know if they would be able to capture/replay at the full 125 Mhz rate.


> What steps have you taken to resolve these difficulties? Any other barriers you have faced to completing this assignment? Mention any students, repos, or other resources you have found helpful in completing the lab so far.
- Attended office hours on Monday, which was helpful, but I was slow to put together the pieces of advice on how to implement the timestamp updates in the PIO
- Attended office hours on Wednesday. Had some questions but didn't get a chance to ask them since I had to leave at the usual end-of-lecture time.
- Sent an email to the instructor with some questions (admittedly, after the original submission deadline).
- Browsed other students' submissions, but did not see any for section 07 that had solved the issues I was facing.
