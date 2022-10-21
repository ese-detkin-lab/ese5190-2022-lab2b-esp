#include <stdio.h>
#include "pico/stdlib.h"
#define LED_PIN 26 //A3

int main() {
    stdio_init_all();
    int i;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    while (true) {

        printf( "Enter a value :\n");
        scanf("%d",&i);
        gpio_put(LED_PIN, i);
        printf( "you print: %d\n", i);
    }
    return 0;
}
