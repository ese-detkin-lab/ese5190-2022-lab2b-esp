
changes to make:
- storage needs to change to allow DMA access
- pre-configure PIO:
    - SM0: all pins in PINCTRL, SM FIFO join RX, program "wait gpio"-> "pio_encode_in(0,32)" 
        - which gpio to wait on? what should trigger level be? need to record transitions.
    - SM1: all pins in pinctrl, SM FIFO join TX, program "pio_encode_out(32)" to write whole register at once, do we need to 
~~- need to add function to configure DMA:~~
~~    Input: , set up DMA~~
- NO need for DMA, just treat PIO FIFO as source/dest register.
    - but then what's the point of using the PIO? Better speed/resolution not seen. DO need DMA


- which "register" to write to gpio? PIO SM FIFO?
- where to read from for reading gpio? PIO SM FIFO?

questions:
- what does it mean to use the PIO to R/W any register? If I'm reading from an internal register and writing to serial, how is PIO involved?
- do I need to implement an I2C engine in PIO? Can I use the .pio program in the pico-examples repo?
    - Think no, just bit-bang it with the sequence. We have time, register, state, so should be accurate enough.
    - is it sufficient to use one PIO SM for each in/out? so have max capacity of 4 pins monitored/written? Would solve problems with trying to selectively write/read values when all 32 GPIO are in PINCTRL. So we'd have a function that configures PIO SM based on commands from serial console, e.g. "read from GPIO 21" also sets up DMA to transfer read values to storage and associate them with the GPIO in question. Or do we just want to read all GPIO values and store a mask of which ones we're concerned about? So all reads will include the register of "SM0 FIFO" and a mask of the ones we're concerned about, and writes will have a register of "SM1 FIFO" and mask of bits we care about. But still, how to only set certain pins if we're setting all "32". Can't do them individually if they're not contiguous.


Monday office hours:
use multiple state machines, one keeps track of timestamps, use interrupts to comm between.
SM0: short 1-2 instruction program, increment counter which is timestamp, push value to FIFO if IRQ is set
SM1: compare pin state to X or Y register, if different, push pin state to FIFO/have DMA push from FIFO to storage array. Want to only have to push value to FIFO, not update register. Can we get another SM to do the updating?
Have one SM do the compare/store and another do the push if IRQ set?
SM2:(?) update X or Y (how to send between SM1 and SM2?)

answer these:
do GPIOs that are set as inputs respond to the PIO setting their output state?
    X/Y question - to prevent the PIO from setting pins we don't care about, just configure those pins as SIO. PIO cannot drive pins not linked to PIO, but can still monitor states.




Thurs night:
Do we use PIO for all register writes? what if we set up DMA from one SM RX FIFO to the WRITE_ADDR of another DMA channel
So this SM stalls waiting for IRQ, when it gets it, it does an `OUT` to it's own ISR, which has autopush enabled. The TX FIFO/OSR of this SM is fed by DMA from a data structure containing addresses.
Another state machine is for values, its TX FIFO/OSR are set up in the same way as the last, DMA from data structure with values.
When it gets an IRQ, it does an `OUT` to its ISR, but this one is hooked up to the DMA channel which has the WRITE_ADDR being controlled by the first SM.
The third state machine generates the IRQ when the timestamp it reads in equals the one it ~~keeps incrementing~~ gets from TIMELR. Where to have DMA put these 2 inputs (timestamp from memory, timer val)?
DMA from TIMELR to TX FIFO of this SM? Then, `OUT` into `X` (autopull).
DMA from struct with saved timestamps to ...? Transfer to `Y`, `JMP X != Y` to `IRQ set`

Similar setup for recording, but can only record pins(?). Same as part of existing [PIO program](rp2040/sequencer.pio), but use DMA to get TIMELR into SM for pushing to storage.
