### TODO:

Create a REPL to let you read and write RP2040 registers from a console. You should be able to:
- select any 32-bit address to read/write (even if not a valid RP2020 address)
- read/write any 32-bit value to this address
- read/write using any of the atomic bit-setting aliases and a 32-bit mask

### My Notes:

Spent a loooooooong time on this one but I think it was worth it.
Features:
- Snazzy UI
- Outputs hex and binary representations of address, value, mask, and masked value
- Can read and write to provided address
- Lets you clear the entered address, value, or mask
- Easily toggle the atomic bit set addresses
- Lemon scented

