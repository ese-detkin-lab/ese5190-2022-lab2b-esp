### TODO:

- modify your sequencer to use the PIO as its primary I/O engine, including the ability to R/W any register 


### My Notes

NOTE: The below was written before the Lab Feedback Questions were posted on Ed and is not my submission for that. I'll edit this to address those specific questions.

TL;DR: I spent most of my time on section 07 on a dead-end design, so I'm laying out my thoughts here in case I don't finish implementing the alternative

I spent a long time trying to craft a workable design that I felt met the design criteria of the assignment, but I don't see a way to do it without compromising some area of the design.
The assignment, as I interpreted it, was to create a sequencer program that did not pre-configure any peripherals (except the PIO and DMA to read/write the PIO FIFOs) or implement any protocol-specific logic which could accept a stream of instructions from the serial console to set up communication with an I2C device. These instructions would be in the form of a register address and a masked value to write to/read from that address, combined with a timestamp representing when that read/write should occur. In this case, the register address would be the PIO state machine FIFO buffers.

The initial plan was to write a collection of PIO programs that would each run on a separate state machine of one PIO module. All "32" GPIO pins would be monitored by the RX programs: One program would be responsible for updating a timestamp and writing that value to the Input Shift Register upon receipt of an interrupt signal. Another would monitor the state of the pins and send an interrupt to the first program when the state changed, then push the pin state to the ISR. Autopush would be enabled to transfer values from both ISRs to the respective FIFO, where DMA would handle transmission to a data structure in the system program.

A similar pair of programs would implement the TX function: One would increment a counter and compare it to a timestamp value read in from the Output Shift Register, setting an interrupt when they are equal. The second program would wait for this interrupt, writing data from its OSR to the pins when it was set.
In a discussion with the professor, he indicated that using DMA to write/read the values directly from memory (without the involvement of the main program) was the goal, as this would offer the best recording/playback fidelity.

I spent a bit on this plan but realized it had a few fatal flaws. First and most critical was that there is no way to increment a value stored in the PIO scratch registers or jump based on the state of an interrupt. A more general flaw was that I didn't see a way to entirely divorce the sequencer from the main program while maintaining the ability to control/read an arbitrary register, especially for bidirectional communication. With the limited instruction set of the PIO, it appears impossible have a single stream of data be responsible for both writing outputs and triggering the reading of inputs, especially if those inputs/outputs were not the GPIO. Additionally, I did not see a way to limit the monitoring of pin states to only those pins specified by the sequence.
I considered pre-processing data read from serial into series of PIO instructions that could be `OUT EXEC`'d when read from memory by PIO, but that doesn't solve the issue with recording/monitoring timestamps, accessing registers internal to the controller, and would be too complex to implement and debug in the time available. 

So, in the interest of my sanity, I decided to compromise on the performance/resolution of the sequencer and implement a "hybrid" solution: 

##### Device Configuration
Pins that will be driven by the PIO should be specified before a sequence is started so they can be associated with the PIO. Pins not linked to the PIO cannot be driven by it, so there is no problem writing a full 32 bits when executing an `OUT PINS` instruction. 

##### Sequence Format
sequence entries provided to the controller by the console program have the following format: 
|R/W|Timestamp|Register Address|Mask|Value|
|---|---------|----------------|----|-----|
| W |001F7736|D0000004|00200000|00000000|

Sequence entries that begin with W result in a write occurring at that timestamp. If the target address is the GPIO pins, this should be specified by addressing the appropriate PIO FIFO buffer. Prior to writing to the pins, a register write should be scheduled that will modify the PIN_CTRL register for the selected state machine to modify SET_BASE to the pin in question and SET_COUNT to 1. The TX PIO program will set the direction of the pin to Output prior to setting the value, and set it back to Input afterward.

Sequence entries that begin with an R will not be added to the sequence in memory, but will instruct the controller to add that address/mask to a list of registers to be monitored in a fast main program loop during a sequence playback. Value transitions in these registers are saved in a separate memory location from the write instructions. The GPIO pins are monitored by a small PIO program which `PUSH`es the state to the FIFO and raises a system interrupt when a pin transition is detected. The interrupt handler moves the value to a data structure that includes the current timestamp.


When exporting data to the serial program, include original sequence writes interleaved with the reads that occurred (or just do it in two separate exports).


If I could provide feedback on this lab, it would be that more well-defined requirements would go a long way to guiding students in the right direction. I imagine there's a tradeoff between providing too much instruction and leaving opportunities for organic discovery, but some definition of the educational goals (write a PIO program? implement DMA to do something? make the most performant program by whatever means?) would help me know which requirements are the most important. 
