#ifndef _WS2812_H
#define _WS2812_H

#include <stdlib.h>
#include <stdio.h>

// void turn_on_power();
// void set_neopixel_color(uint32_t color); 

void turn_on_pixel();
void set_pixel_color(uint32_t rgb);
void rp_init();


#endif