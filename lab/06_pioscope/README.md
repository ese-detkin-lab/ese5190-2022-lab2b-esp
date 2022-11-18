### How it works
- DMA maintains a 'counter' for each channel. Each 1-clock pulse for the 'dreq' signal will increment the 'counter'. \ When nonzero, the channel will request transfer from the DMA's internal arbiter and decrease the value of the counter when the transfered is issued to address FIFOs.
- There are two PIO: each can be used for any GPIOs
- Memory -> TXF(auto `PULL`) -> OSR(`OUT`) -> GPIO <br> Memory <- RXF(auto `PUSH`) <- ISR(`IN`) <- GPIO
- With `GPIO_IN`, all 30 GPIO values are returned in a single read. Software can mask out individual pins that the program.task requires.
