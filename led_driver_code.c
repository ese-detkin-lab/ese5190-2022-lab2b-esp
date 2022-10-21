/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "C:\picoo\pico-sdk\src\boards\include\boards\adafruit_qtpy_rp2040.h"

#define PICO_PIN 23
int main() {
    const uint LED_PIN = PICO_PIN;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    
    stdio_init_all();
    while (true) 
    {
        int c;
        scanf("%d",&c);
        
        if(c>5)
            {
                printf("It is ON, give a number <5 to stop\n");    
            gpio_put(LED_PIN, 1 );
            }
        if(c <5)
            { 
                printf("It is OFF, give a number >5 to start\n"); 
                gpio_put(LED_PIN, 0);
            }
    }
}
