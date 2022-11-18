#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"

#include "ws2812.pio.h"
#define PIO			pio0
#define SM			0
#define FREQ		800000
#define POW_PIN		11
#define IS_RGBW		true
#ifdef PICO_DEFAULT_WS2812_PIN
#define WS2812_PIN PICO_DEFAULT_WS2812_PIN
#else
// default to pin 2 if the board doesn't have a default WS2812 pin defined
#define WS2812_PIN 12
#endif

static inline void put_pixel(uint32_t pixel_grb) {
    pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
}

static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b) {
    return
            ((uint32_t) (r) << 16) |
            ((uint32_t) (g) << 8) |
            (uint32_t) (b);
}

#define GPIO_REGISTER_BASE 0x40014000

int bbutton_press(){

    volatile uint32_t* address=GPIO_REGISTER_BASE+(uint32_t) 0x0A8;

    printf("Boot buttonpress check. On? \n");
    printf("%08x\n",*address);
    return *address;
}

void set_neopixel_color(uint32_t color){
   
    uint32_t r= color>>16;
    uint32_t g= color>>8;
    uint32_t b= color;
    put_pixel(urgb_u32(r, g, b));
}

void init_neopixel()
{
	gpio_init(POW_PIN);
    gpio_set_dir(POW_PIN, GPIO_OUT);
    gpio_put(POW_PIN,1);
	
    uint offset = pio_add_program(PIO, &ws2812_program);
    ws2812_program_init(PIO, SM, offset, WS2812_PIN, FREQ, IS_RGBW);
}

int main() {
    stdio_init_all();
	init_neopixel();
	
	int bpress = 0;
    int t = 0;
    while (1) {
		bpress = bbutton_press();
		t = (!bpress)? t+1: t;
		//printf("Hello world!");
		if (!(t%2))
		{
			put_pixel(urgb_u32(0x77,0x77,0x11));
			sleep_ms(50);
			put_pixel(0);
			printf("Yes!\nYellow blinks!!\n");
		}
		else
		{
			put_pixel(0);
			sleep_ms(50);
        }
		sleep_ms(50);
    }
}
