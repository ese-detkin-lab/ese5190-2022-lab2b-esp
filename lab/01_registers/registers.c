#include "pico/stdlib.h"
#include <stdio.h>
#include "neopixel.h"
#include "hardware/gpio.h"

#define QTPY_BOOT_PIN 21

#define QTPY_BOOT_PIN_BITMASK 0x200000

#define GPIO_IN_OFFSET 0x4

int main() {
    volatile uint32_t* address = SIO_BASE | SIO_GPIO_IN_OFFSET;
    stdio_init_all();
    gpio_init(QTPY_BOOT_PIN);
    gpio_set_dir(QTPY_BOOT_PIN, GPIO_IN);
    neopixel_init();

    sleep_ms(5000);
    printf("address: %X\n", address);
    while (true) {
        printf("value: %08X", *address);
        printf("\tmask: %08X", QTPY_BOOT_PIN_BITMASK);
        printf("\t masked: %08x", (*address & QTPY_BOOT_PIN_BITMASK));
        if (!!!(*address & QTPY_BOOT_PIN_BITMASK)) { // poll every cycle, 0 = "pressed"
            neopixel_set_rgb(0x1);
            printf("\tpressed!");
        } else {
            neopixel_set_rgb(0x00000000);
        }
        printf("\n");
        sleep_ms(100); // don't DDOS the serial console
    }
    return 0;
}                  
