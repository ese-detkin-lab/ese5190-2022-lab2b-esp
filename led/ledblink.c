/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "pico/stdlib.h"
#include "stdio.h"
#include "hardware/gpio.h"
#include "adafruit_qtpy_rp2040.h"

#

int main() {
stdio_init_all();
#ifndef PICO_DEFAULT_LED_PIN
//#ifndef PICO_LED_PIN=25
#warning blink example requires a board with a regular LED
#else
    const uint LED_PIN = PICO_DEFAULT_LED_PIN;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    while (true) {
        gpio_put(LED_PIN, 1);
        sleep_ms(400);
        printf("LED Turned ON!\n");
        sleep_ms(400);
        gpio_put(LED_PIN, 0);
        sleep_ms(400);
        printf("LED Turned OFF!\n");
        sleep_ms(400);
    }
#endif
}
