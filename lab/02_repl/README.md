### TODO:

Create a REPL to let you read and write RP2040 registers from a console. You should be able to:
- select any 32-bit address to read/write (even if not a valid RP2020 address)
- read/write any 32-bit value to this address
- read/write using any of the atomic bit-setting aliases and a 32-bit mask

## Code
```
#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "ws2812.h"

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "ws2812.pio.h"
#include "registers.h"

#define IS_RGBW true
#define NUM_PIXELS 150

#ifdef PICO_DEFAULT_WS2812_PIN
#define WS2812_PIN PICO_DEFAULT_WS2812_PIN
#else

// default to pin 2 if the board doesn't have a default WS2812 pin defined

#define WS2812_PIN 12
#define WS2812_POWER_PIN 11
#endif



int main() {
    stdio_init_all();

    PIO pio = pio0;
    int sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);
    turn_on_power();
    ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, IS_RGBW);


    uint32_t input_address = 0x00000000;
    ADDRESS address = 0x00000000;
    VALUE value;
    int mode;

    while (true) {
        printf("Start. \n");

        // address selection
        printf("Enter an address you want to read/write: \n");
        scanf("%x", &input_address);  
        printf("Address is %x\n",input_address);
        address = (ADDRESS) input_address;

        // mode selection
        printf("Enter a mode you want to operate (0 for read/1 for write): \n");
        scanf("%d", &mode);
        
        // reading mode
        if(mode == 0) {
            printf("The data read is: %x\n", register_read(address));
        }
        // writing mode
        else {
            // value input
            printf("Enter an value you want to write in: \n");
            scanf("%x", &value);  
            printf("Value is %x \n",value);
            register_write(address, value);
            printf("The value written in: %x\n", register_read(address));
        }
        sleep_ms(250);

    }

    return 0;
    }
    
   ## Result
   Type the register address. Choose the mode you want to read or write.
   The following picture shows we write the value 1 into adress 0x d0000060
   ![image](https://user-images.githubusercontent.com/114196821/201794923-c87f1baa-0678-4465-bd29-03718e2a99dc.PNG)
   <img width="494" alt="labpic" src="https://user-images.githubusercontent.com/114196821/201794923-c87f1baa-0678-4465-bd29-03718e2a99dc.PNG">
   Besides, the following picture shows I read the value in rigister 0x 400140a8 which shows the status of GPIO21 (Button). When I press the button, the read value is 0. When I release the button, the read value is 70e0000
   <img width="494" alt="lab" src="https://user-images.githubusercontent.com/114196821/201795425-77eeb37e-853e-49f8-8984-66c35f2ae305.PNG">

   <img width="494" alt="lab" src="https://user-images.githubusercontent.com/114196821/201795478-080740f5-af65-4f9f-b460-44f3d5b8ac5e.PNG">

   
