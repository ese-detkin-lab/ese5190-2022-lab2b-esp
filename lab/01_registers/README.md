### TODO:

Toggle the Qt Py's Neopixel LED when the BOOT button is pressed, using only direct register reads to access the boot button status. You may use the SDK to initialize the board and the pins, and the WS2812 example code to toggle the Neopixel. 

### Registers.c:

#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "ws2812.h"
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "ws2812.pio.h"

#define IS_RGBW true
#define NUM_PIXELS 150

#ifdef PICO_DEFAULT_WS2812_PIN
#define WS2812_PIN PICO_DEFAULT_WS2812_PIN
#else

// default to pin 2 if the board doesn't have a default WS2812 pin defined

#define WS2812_PIN 12
#define WS2812_POWER_PIN 11
#endif

int main()
{
    
    stdio_init_all();

    PIO pio = pio0;
    int sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);
    turn_on_power();
    ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, IS_RGBW);

    const uint BOOT_PIN = 21;
    // the BOOT button is GPIO21
    gpio_init(BOOT_PIN);
    gpio_set_dir(BOOT_PIN, GPIO_IN);
    // set BOOT_PIN as input
    uint32_t pixel_grb = 0x00000000;
    while (1) {
        if(!gpio_get(BOOT_PIN)) { 
            // use gpio_get() to access the boot button status
            pixel_grb = 0X00FFFFFF;
        } else {
            pixel_grb = 0x00000000;
        }
        set_neopixel_color(pixel_grb);
        sleep_ms(100);
    }
    return 0;
}
