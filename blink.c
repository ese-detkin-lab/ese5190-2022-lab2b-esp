#include <stdio.h>
#include "pico/stdlib.h"
#define LED_PIN 26 //A3

int main() {
    stdio_init_all();
    int i;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    while (true) {
        gpio_put(LED_PIN, 1);
        printf( "True\n");
        sleep_ms(500);
        gpio_put(LED_PIN, 0);
        sleep_ms(500);
        printf( "False\n");
    }
    return 0;
}
