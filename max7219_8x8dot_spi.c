/**
 * Copyright (c) 2022 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/spi.h"

/* Example code to talk to a Max7219 driving an 8 digit 8 segment display via SPI

   NOTE: The device is driven at 5v, but SPI communications are at 3v3

   Connections on Raspberry Pi Pico board and a generic Max7219 board, other
   boards may vary.

   * GPIO 17 (pin 22) Chip select -> CS on Max7219 board
   * GPIO 18 (pin 24) SCK/spi0_sclk -> CLK on Max7219 board
   * GPIO 19 (pin 25) MOSI/spi0_tx -> DIN on Max7219 board
   * 5v (pin 40) -> VCC on Max7219 board
   * GND (pin 38)  -> GND on Max7219 board

   Note: SPI devices can have a number of different naming schemes for pins. See
   the Wikipedia page at https://en.wikipedia.org/wiki/Serial_Peripheral_Interface
   for variations.

*/

// This defines how many Max7219 modules we have cascaded together, in this case, just the one.
#define NUM_MODULES 1

void greet();
void nichts();
void numbers();
void alphabets();
void shapes();
void fill_shapes();
void emojis();
void hearts();
void pacman();
void squares();
void circle();
void triangles();
void rectangles();
void f_squares();
void f_circle();
void f_triangles();
void f_rectangles();
void em_smiles();
void em_sad();
void em_speechless();
void em_ouch();
void zero();
void one();
void two();
void three();
void four();
void five();
void six();
void seven();
void eight();
void nine();

const uint8_t CMD_NOOP = 0;
const uint8_t CMD_DIGIT0 = 1; // Goes up to 8, for each line
const uint8_t CMD_DECODEMODE = 9;
const uint8_t CMD_BRIGHTNESS = 10;
const uint8_t CMD_SCANLIMIT = 11;
const uint8_t CMD_SHUTDOWN = 12;
const uint8_t CMD_DISPLAYTEST = 15;

#define PICO_DEFAULT_SPI_CSN_PIN 5
#define PICO_DEFAULT_SPI_SCK_PIN 6
#define PICO_DEFAULT_SPI_TX_PIN 3

#define LED1_PIN 29
#define LED2_PIN 28
#define LED3_PIN 27
#define LED4_PIN 26
#define LED5_PIN 24
#define LED6_PIN 25
#define LED7_PIN 20

#ifdef PICO_DEFAULT_SPI_CSN_PIN
static inline void cs_select() {
    asm volatile("nop \n nop \n nop");
    gpio_put(PICO_DEFAULT_SPI_CSN_PIN, 0);  // Active low
    asm volatile("nop \n nop \n nop");
}

static inline void cs_deselect() {
    asm volatile("nop \n nop \n nop");
    gpio_put(PICO_DEFAULT_SPI_CSN_PIN, 1);
    asm volatile("nop \n nop \n nop");
}
#endif

#if defined(spi_default) && defined(PICO_DEFAULT_SPI_CSN_PIN)
static void write_register(uint8_t reg, uint8_t data) {
    uint8_t buf[2];
    buf[0] = reg;
    buf[1] = data;
    cs_select();
    spi_write_blocking(spi_default, buf, 2);
    cs_deselect();
    sleep_ms(1);
}

static void write_register_all(uint8_t reg, uint8_t data) {
    uint8_t buf[2];
    buf[0] = reg;
    buf[1] = data;
    cs_select();
    for (int i = 0; i< NUM_MODULES;i++) {
        spi_write_blocking(spi_default, buf, 2);
    }
    cs_deselect();
}
#endif

void clear()
{
    for (int i=0;i<=8;i++) {
        write_register_all(CMD_DIGIT0 + i, 0);
    }
	gpio_put(LED1_PIN, 0);
	gpio_put(LED2_PIN, 0);
	gpio_put(LED3_PIN, 0);
	gpio_put(LED4_PIN, 0);
	gpio_put(LED5_PIN, 0);
	gpio_put(LED6_PIN, 0);
	gpio_put(LED7_PIN, 0);
}

int main() {
    stdio_init_all();

#if !defined(spi_default) || !defined(PICO_DEFAULT_SPI_SCK_PIN) || !defined(PICO_DEFAULT_SPI_TX_PIN) || !defined(PICO_DEFAULT_SPI_CSN_PIN)
#warning spi/max7219_8x7seg_spi example requires a board with SPI pins
    printf("Default SPI pins were not defined");
#else

    printf("Hello, max7219! Drawing things on a 8 x 8 segment display since 2022...\n");

    // This example will use SPI0 at 10MHz.
    spi_init(spi_default, 10 * 1000 * 1000);
    gpio_set_function(PICO_DEFAULT_SPI_SCK_PIN, GPIO_FUNC_SPI);
    gpio_set_function(PICO_DEFAULT_SPI_TX_PIN, GPIO_FUNC_SPI);

    // Chip select is active-low, so we'll initialise it to a driven-high state
    gpio_init(PICO_DEFAULT_SPI_CSN_PIN);
    gpio_set_dir(PICO_DEFAULT_SPI_CSN_PIN, GPIO_OUT);
    gpio_put(PICO_DEFAULT_SPI_CSN_PIN, 1);

    // Send init sequence to device
    write_register_all(CMD_SHUTDOWN, 0);
    write_register_all(CMD_DISPLAYTEST, 0);
    write_register_all(CMD_SCANLIMIT, 7);
    write_register_all(CMD_DECODEMODE, 255);
    write_register_all(CMD_SHUTDOWN, 1);
    write_register_all(CMD_BRIGHTNESS, 8);
	
	gpio_init(LED1_PIN);
    gpio_set_dir(LED1_PIN, GPIO_OUT);
	gpio_init(LED2_PIN);
    gpio_set_dir(LED2_PIN, GPIO_OUT);
	gpio_init(LED3_PIN);
    gpio_set_dir(LED3_PIN, GPIO_OUT);
	gpio_init(LED4_PIN);
    gpio_set_dir(LED4_PIN, GPIO_OUT);
	gpio_init(LED5_PIN);
    gpio_set_dir(LED5_PIN, GPIO_OUT);
	gpio_init(LED6_PIN);
    gpio_set_dir(LED6_PIN, GPIO_OUT);
	gpio_init(LED7_PIN);
    gpio_set_dir(LED7_PIN, GPIO_OUT);

    clear();
	char chc = '0';

    while (true) {
		printf("\n\nMENU");
		greet();
		//sleep_ms(1000);
		printf("\n\ta. Alphabets");
		printf("\n\tb. Numbers");
		printf("\n\tc. Shapes");
		printf("\n\td. Filled Shapes");
		printf("\n\te. Emojis");
		printf("\n\tf. Hearts");
		printf("\n\tg. Pacman");
		printf("\n\th. Traffic Lights");
		printf("\n\ti. RGB");
		printf("\nWhich one would you like to see on 8x8 dot matrix? ");
		scanf("%c", &chc);
		clear();
		sleep_ms(500);
		switch(chc)
		{
			case 'a': alphabets();
					sleep_ms(1000);
					clear();
					sleep_ms(500);
					break;
			case 'b': numbers();
					sleep_ms(1000);
					clear();
					sleep_ms(500);
					break;
			case 'c': shapes();
					sleep_ms(1000);
					clear();
					sleep_ms(500);
					break;
			case 'd': fill_shapes();
					sleep_ms(1000);
					clear();
					sleep_ms(500);
					break;
			case 'e': emojis();
					sleep_ms(1000);
					clear();
					sleep_ms(500);
					break;
			case 'f': hearts();
					sleep_ms(1000);
					clear();
					sleep_ms(500);
					break;
			case 'g': pacman();
					sleep_ms(1000);
					clear();
					sleep_ms(500);
					break;
			case 'h': seven();
					gpio_put(LED5_PIN, 1);
					gpio_put(LED6_PIN, 0);
					gpio_put(LED7_PIN, 0);
					sleep_ms(3000);
					gpio_put(LED5_PIN, 0);
					gpio_put(LED6_PIN, 1);
					gpio_put(LED7_PIN, 0);
					sleep_ms(1500);
					gpio_put(LED5_PIN, 0);
					gpio_put(LED6_PIN, 0);
					gpio_put(LED7_PIN, 1);
					sleep_ms(3000);
					clear();
					sleep_ms(500);
					break;
			case 'i': eight();
					gpio_put(LED1_PIN, 1);
					gpio_put(LED2_PIN, 0);
					gpio_put(LED3_PIN, 0);
					sleep_ms(2000);
					gpio_put(LED1_PIN, 0);
					gpio_put(LED2_PIN, 1);
					gpio_put(LED3_PIN, 0);
					sleep_ms(2000);
					gpio_put(LED1_PIN, 0);
					gpio_put(LED2_PIN, 0);
					gpio_put(LED3_PIN, 1);
					sleep_ms(2000);
					gpio_put(LED1_PIN, 1);
					gpio_put(LED2_PIN, 1);
					gpio_put(LED3_PIN, 0);
					sleep_ms(2000);
					gpio_put(LED1_PIN, 0);
					gpio_put(LED2_PIN, 1);
					gpio_put(LED3_PIN, 1);
					sleep_ms(2000);
					gpio_put(LED1_PIN, 1);
					gpio_put(LED2_PIN, 0);
					gpio_put(LED3_PIN, 1);
					sleep_ms(2000);
					gpio_put(LED1_PIN, 1);
					gpio_put(LED2_PIN, 1);
					gpio_put(LED3_PIN, 1);
					sleep_ms(2000);
					clear();
					sleep_ms(500);
					break;
			default: nichts();
					gpio_put(LED4_PIN, 1);
					sleep_ms(2000);
					clear();
					sleep_ms(500);
					break;
		}		
    }

    return 0;
#endif
}

void nichts()
{
	write_register_all(0x1, 0x42);
	write_register_all(0x2, 0x81);
	write_register_all(0x3, 0x00);
	write_register_all(0x4, 0x00);
	write_register_all(0x5, 0x00);
	write_register_all(0x6, 0x00);
	write_register_all(0x7, 0x81);
	write_register_all(0x8, 0x42);
}

void greet()
{
	write_register_all(0x1, 0x89);
	write_register_all(0x2, 0x89);
	write_register_all(0x3, 0x89);
	write_register_all(0x4, 0xf9);
	write_register_all(0x5, 0x89);
	write_register_all(0x6, 0x89);
	write_register_all(0x7, 0x89);
	write_register_all(0x8, 0x00);
}

void hearts()
{
	write_register_all(0x1, 0x66);
	write_register_all(0x2, 0x99);
	write_register_all(0x3, 0x81);
	write_register_all(0x4, 0x81);
	write_register_all(0x5, 0x81);
	write_register_all(0x6, 0x42);
	write_register_all(0x7, 0x24);
	write_register_all(0x8, 0x18);
	sleep_ms(1000);
	write_register_all(0x1, 0x66);
	write_register_all(0x2, 0xff);
	write_register_all(0x3, 0xff);
	write_register_all(0x4, 0xff);
	write_register_all(0x5, 0xff);
	write_register_all(0x6, 0x7e);
	write_register_all(0x7, 0x3c);
	write_register_all(0x8, 0x18);
}

void pacman()
{
	write_register_all(0x1, 0x38);
	write_register_all(0x2, 0x44);
	write_register_all(0x3, 0x88);
	write_register_all(0x4, 0x90);
	write_register_all(0x5, 0x88);
	write_register_all(0x6, 0x44);
	write_register_all(0x7, 0x38);
	write_register_all(0x8, 0x00);
	sleep_ms(1000);
	write_register_all(0x1, 0x38);
	write_register_all(0x2, 0x44);
	write_register_all(0x3, 0x88);
	write_register_all(0x4, 0x91);
	write_register_all(0x5, 0x88);
	write_register_all(0x6, 0x44);
	write_register_all(0x7, 0x38);
	write_register_all(0x8, 0x00);
	sleep_ms(1000);
	write_register_all(0x1, 0x38);
	write_register_all(0x2, 0x44);
	write_register_all(0x3, 0x88);
	write_register_all(0x4, 0x92);
	write_register_all(0x5, 0x88);
	write_register_all(0x6, 0x44);
	write_register_all(0x7, 0x38);
	write_register_all(0x8, 0x00);
	sleep_ms(1000);
	write_register_all(0x1, 0x38);
	write_register_all(0x2, 0x44);
	write_register_all(0x3, 0x88);
	write_register_all(0x4, 0x94);
	write_register_all(0x5, 0x88);
	write_register_all(0x6, 0x44);
	write_register_all(0x7, 0x38);
	write_register_all(0x8, 0x00);
	sleep_ms(1000);
	write_register_all(0x1, 0x38);
	write_register_all(0x2, 0x44);
	write_register_all(0x3, 0x88);
	write_register_all(0x4, 0x98);
	write_register_all(0x5, 0x88);
	write_register_all(0x6, 0x44);
	write_register_all(0x7, 0x38);
	write_register_all(0x8, 0x00);
	sleep_ms(1000);
	write_register_all(0x1, 0x38);
	write_register_all(0x2, 0x7c);
	write_register_all(0x3, 0xf8);
	write_register_all(0x4, 0xf0);
	write_register_all(0x5, 0xf8);
	write_register_all(0x6, 0x7c);
	write_register_all(0x7, 0x38);
	write_register_all(0x8, 0x00);
}

void em_smiles()
{
    write_register_all(0x1, 0x00);
	write_register_all(0x2, 0x24);
	write_register_all(0x3, 0x24);
	write_register_all(0x4, 0x00);
	write_register_all(0x5, 0x81);
	write_register_all(0x6, 0x42);
	write_register_all(0x7, 0x3c);
	write_register_all(0x8, 0x00);
	sleep_ms(1000);
	write_register_all(0x1, 0x00);
	write_register_all(0x2, 0x24);
	write_register_all(0x3, 0x24);
	write_register_all(0x4, 0x24);
	write_register_all(0x5, 0x00);
	write_register_all(0x6, 0x42);
	write_register_all(0x7, 0x3c);
	write_register_all(0x8, 0x00);
	sleep_ms(1000);
	write_register_all(0x1, 0x3c);
	write_register_all(0x2, 0x42);
	write_register_all(0x3, 0xa5);
	write_register_all(0x4, 0x81);
	write_register_all(0x5, 0xa5);
	write_register_all(0x6, 0x99);
	write_register_all(0x7, 0x42);
	write_register_all(0x8, 0x3c);
}
void em_sad()
{
    write_register_all(0x1, 0x3c);
	write_register_all(0x2, 0x42);
	write_register_all(0x3, 0xa5);
	write_register_all(0x4, 0x81);
	write_register_all(0x5, 0x99);
	write_register_all(0x6, 0xa5);
	write_register_all(0x7, 0x42);
	write_register_all(0x8, 0x3c);
}
void em_speechless()
{
    write_register_all(0x1, 0x3c);
	write_register_all(0x2, 0x42);
	write_register_all(0x3, 0xa5);
	write_register_all(0x4, 0x81);
	write_register_all(0x5, 0xbd);
	write_register_all(0x6, 0x81);
	write_register_all(0x7, 0x42);
	write_register_all(0x8, 0x3c);
}
void em_ouch()
{
    write_register_all(0x1, 0x3c);
	write_register_all(0x2, 0x42);
	write_register_all(0x3, 0xa5);
	write_register_all(0x4, 0x81);
	write_register_all(0x5, 0xbd);
	write_register_all(0x6, 0xbd);
	write_register_all(0x7, 0x42);
	write_register_all(0x8, 0x3c);
	sleep_ms(1000);
	write_register_all(0x1, 0xff);
	write_register_all(0x2, 0x81);
	write_register_all(0x3, 0xa5);
	write_register_all(0x4, 0x81);
	write_register_all(0x5, 0xbd);
	write_register_all(0x6, 0xbd);
	write_register_all(0x7, 0x81);
	write_register_all(0x8, 0xff);
}

void squares()
{
	write_register_all(0x1, 0xff);
	write_register_all(0x2, 0x81);
	write_register_all(0x3, 0xbd);
	write_register_all(0x4, 0xa5);
	write_register_all(0x5, 0xa5);
	write_register_all(0x6, 0xbd);
	write_register_all(0x7, 0x81);
	write_register_all(0x8, 0xff);
}

void circle()
{
	write_register_all(0x1, 0x3c);
	write_register_all(0x2, 0x42);
	write_register_all(0x3, 0x81);
	write_register_all(0x4, 0x81);
	write_register_all(0x5, 0x81);
	write_register_all(0x6, 0x81);
	write_register_all(0x7, 0x42);
	write_register_all(0x8, 0x3c);
}

void triangles()
{
	write_register_all(0x1, 0x04);
	write_register_all(0x2, 0x0a);
	write_register_all(0x3, 0x1f);
	write_register_all(0x4, 0x00);
	write_register_all(0x5, 0x00);
	write_register_all(0x6, 0x00);
	write_register_all(0x7, 0x00);
	write_register_all(0x8, 0x00);
	sleep_ms(1000);
	write_register_all(0x1, 0x10);
	write_register_all(0x2, 0x28);
	write_register_all(0x3, 0x44);
	write_register_all(0x4, 0xfe);
	write_register_all(0x5, 0x00);
	write_register_all(0x6, 0x00);
	write_register_all(0x7, 0x00);
	write_register_all(0x8, 0x00);
	sleep_ms(1000);
	write_register_all(0x1, 0x01);
	write_register_all(0x2, 0x03);
	write_register_all(0x3, 0x05);
	write_register_all(0x4, 0x09);
	write_register_all(0x5, 0x11);
	write_register_all(0x6, 0x21);
	write_register_all(0x7, 0x41);
	write_register_all(0x8, 0xff);
}

void rectangles()
{
	write_register_all(0x1, 0x00);
	write_register_all(0x2, 0x00);
	write_register_all(0x3, 0xff);
	write_register_all(0x4, 0x81);
	write_register_all(0x5, 0x81);
	write_register_all(0x6, 0xff);
	write_register_all(0x7, 0x00);
	write_register_all(0x8, 0x00);
	sleep_ms(1000);
	write_register_all(0x1, 0x00);
	write_register_all(0x2, 0xff);
	write_register_all(0x3, 0x81);
	write_register_all(0x4, 0x81);
	write_register_all(0x5, 0x81);
	write_register_all(0x6, 0x81);
	write_register_all(0x7, 0xff);
	write_register_all(0x8, 0x00);
	sleep_ms(1000);
	write_register_all(0x1, 0x7e);
	write_register_all(0x2, 0x42);
	write_register_all(0x3, 0x42);
	write_register_all(0x4, 0x42);
	write_register_all(0x5, 0x42);
	write_register_all(0x6, 0x42);
	write_register_all(0x7, 0x42);
	write_register_all(0x8, 0x7e);
	sleep_ms(1000);
	write_register_all(0x1, 0x3c);
	write_register_all(0x2, 0x24);
	write_register_all(0x3, 0x24);
	write_register_all(0x4, 0x24);
	write_register_all(0x5, 0x24);
	write_register_all(0x6, 0x24);
	write_register_all(0x7, 0x24);
	write_register_all(0x8, 0x3c);
}

void f_squares()
{
	write_register_all(0x1, 0xff);
	write_register_all(0x2, 0x81);
	write_register_all(0x3, 0xbd);
	write_register_all(0x4, 0xbd);
	write_register_all(0x5, 0xbd);
	write_register_all(0x6, 0xbd);
	write_register_all(0x7, 0x81);
	write_register_all(0x8, 0xff);
}

void f_circle()
{
	write_register_all(0x1, 0x3c);
	write_register_all(0x2, 0x7e);
	write_register_all(0x3, 0xff);
	write_register_all(0x4, 0xff);
	write_register_all(0x5, 0xff);
	write_register_all(0x6, 0xff);
	write_register_all(0x7, 0x7e);
	write_register_all(0x8, 0x3c);
}

void f_triangles()
{
	write_register_all(0x1, 0x04);
	write_register_all(0x2, 0x0e);
	write_register_all(0x3, 0x1f);
	write_register_all(0x4, 0x00);
	write_register_all(0x5, 0x00);
	write_register_all(0x6, 0x00);
	write_register_all(0x7, 0x00);
	write_register_all(0x8, 0x00);
	sleep_ms(1000);
	write_register_all(0x1, 0x10);
	write_register_all(0x2, 0x38);
	write_register_all(0x3, 0x7c);
	write_register_all(0x4, 0xfe);
	write_register_all(0x5, 0x00);
	write_register_all(0x6, 0x00);
	write_register_all(0x7, 0x00);
	write_register_all(0x8, 0x00);
	sleep_ms(1000);
	write_register_all(0x1, 0x01);
	write_register_all(0x2, 0x03);
	write_register_all(0x3, 0x07);
	write_register_all(0x4, 0x0f);
	write_register_all(0x5, 0x1f);
	write_register_all(0x6, 0x3f);
	write_register_all(0x7, 0x7f);
	write_register_all(0x8, 0xff);
}

void f_rectangles()
{
	write_register_all(0x1, 0x00);
	write_register_all(0x2, 0x00);
	write_register_all(0x3, 0xff);
	write_register_all(0x4, 0xff);
	write_register_all(0x5, 0xff);
	write_register_all(0x6, 0xff);
	write_register_all(0x7, 0x00);
	write_register_all(0x8, 0x00);
	sleep_ms(1000);
	write_register_all(0x1, 0x00);
	write_register_all(0x2, 0xff);
	write_register_all(0x3, 0xff);
	write_register_all(0x4, 0xff);
	write_register_all(0x5, 0xff);
	write_register_all(0x6, 0xff);
	write_register_all(0x7, 0xff);
	write_register_all(0x8, 0x00);
	sleep_ms(1000);
	write_register_all(0x1, 0x7e);
	write_register_all(0x2, 0x7e);
	write_register_all(0x3, 0x7e);
	write_register_all(0x4, 0x7e);
	write_register_all(0x5, 0x7e);
	write_register_all(0x6, 0x7e);
	write_register_all(0x7, 0x7e);
	write_register_all(0x8, 0x7e);
	sleep_ms(1000);
	write_register_all(0x1, 0x3c);
	write_register_all(0x2, 0x3c);
	write_register_all(0x3, 0x3c);
	write_register_all(0x4, 0x3c);
	write_register_all(0x5, 0x3c);
	write_register_all(0x6, 0x3c);
	write_register_all(0x7, 0x3c);
	write_register_all(0x8, 0x3c);
}

void emojis()
{
	em_smiles();
	sleep_ms(1000);
	em_sad();
	sleep_ms(1000);
	em_speechless();
	sleep_ms(1000);
	em_ouch();
}

void shapes()
{
    squares();
	sleep_ms(1000);
	circle();
	sleep_ms(1000);
	triangles();
	sleep_ms(1000);
	rectangles();
}

void fill_shapes()
{
    f_squares();
	sleep_ms(1000);
	f_circle();
	sleep_ms(1000);
	f_triangles();
	sleep_ms(1000);
	f_rectangles();
}
void zero()
{
	write_register_all(0x1, 0x0e);
	write_register_all(0x2, 0x11);
	write_register_all(0x3, 0x11);
	write_register_all(0x4, 0x11);
	write_register_all(0x5, 0x11);
	write_register_all(0x6, 0x11);
	write_register_all(0x7, 0x0e);
	write_register_all(0x8, 0x00);
}
void one()
{
	write_register_all(0x1, 0x04);
	write_register_all(0x2, 0x0c);
	write_register_all(0x3, 0x04);
	write_register_all(0x4, 0x04);
	write_register_all(0x5, 0x04);
	write_register_all(0x6, 0x04);
	write_register_all(0x7, 0x04);
	write_register_all(0x8, 0x00);
}
void two()
{
	write_register_all(0x1, 0x0e);
	write_register_all(0x2, 0x11);
	write_register_all(0x3, 0x01);
	write_register_all(0x4, 0x02);
	write_register_all(0x5, 0x04);
	write_register_all(0x6, 0x08);
	write_register_all(0x7, 0x1f);
	write_register_all(0x8, 0x00);
}
void three()
{
	write_register_all(0x1, 0x0e);
	write_register_all(0x2, 0x11);
	write_register_all(0x3, 0x11);
	write_register_all(0x4, 0x06);
	write_register_all(0x5, 0x01);
	write_register_all(0x6, 0x11);
	write_register_all(0x7, 0x0e);
	write_register_all(0x8, 0x00);
}
void four()
{
	write_register_all(0x1, 0x03);
	write_register_all(0x2, 0x05);
	write_register_all(0x3, 0x09);
	write_register_all(0x4, 0x11);
	write_register_all(0x5, 0x1f);
	write_register_all(0x6, 0x01);
	write_register_all(0x7, 0x01);
	write_register_all(0x8, 0x00);
}
void five()
{
	write_register_all(0x1, 0x0f);
	write_register_all(0x2, 0x10);
	write_register_all(0x3, 0x10);
	write_register_all(0x4, 0x1e);
	write_register_all(0x5, 0x01);
	write_register_all(0x6, 0x01);
	write_register_all(0x7, 0x1e);
	write_register_all(0x8, 0x00);
}
void six()
{
	write_register_all(0x1, 0x0e);
	write_register_all(0x2, 0x10);
	write_register_all(0x3, 0x10);
	write_register_all(0x4, 0x1e);
	write_register_all(0x5, 0x11);
	write_register_all(0x6, 0x11);
	write_register_all(0x7, 0x0e);
	write_register_all(0x8, 0x00);
}
void seven()
{
	write_register_all(0x1, 0x1e);
	write_register_all(0x2, 0x01);
	write_register_all(0x3, 0x01);
	write_register_all(0x4, 0x02);
	write_register_all(0x5, 0x04);
	write_register_all(0x6, 0x08);
	write_register_all(0x7, 0x10);
	write_register_all(0x8, 0x00);
}
void eight()
{
	write_register_all(0x1, 0x0e);
	write_register_all(0x2, 0x11);
	write_register_all(0x3, 0x11);
	write_register_all(0x4, 0x1f);
	write_register_all(0x5, 0x11);
	write_register_all(0x6, 0x11);
	write_register_all(0x7, 0x0e);
	write_register_all(0x8, 0x00);
}
void nine()
{
	write_register_all(0x1, 0x0e);
	write_register_all(0x2, 0x11);
	write_register_all(0x3, 0x11);
	write_register_all(0x4, 0x0f);
	write_register_all(0x5, 0x01);
	write_register_all(0x6, 0x01);
	write_register_all(0x7, 0x0e);
	write_register_all(0x8, 0x00);
}
void numbers()
{
	zero();
	sleep_ms(1000);
	one();
	sleep_ms(1000);
	two();
	sleep_ms(1000);
	three();
	sleep_ms(1000);
	four();
	sleep_ms(1000);
	five();
	sleep_ms(1000);
	six();
	sleep_ms(1000);
	seven();
	sleep_ms(1000);
	eight();
	sleep_ms(1000);
	nine();
}

void alphabets()
{
    //AB
	write_register_all(0x1, 0x6e);
	write_register_all(0x2, 0x99);
	write_register_all(0x3, 0x99);
	write_register_all(0x4, 0x99);
	write_register_all(0x5, 0xfe);
	write_register_all(0x6, 0x99);
	write_register_all(0x7, 0x99);
	write_register_all(0x8, 0x9e);
	sleep_ms(1000);
	//A
	write_register_all(0x1, 0x0e);
	write_register_all(0x2, 0x11);
	write_register_all(0x3, 0x11);
	write_register_all(0x4, 0x1f);
	write_register_all(0x5, 0x11);
	write_register_all(0x6, 0x11);
	write_register_all(0x7, 0x11);
	write_register_all(0x8, 0x00);
	sleep_ms(1000);
	//B
	write_register_all(0x1, 0x1e);
	write_register_all(0x2, 0x11);
	write_register_all(0x3, 0x11);
	write_register_all(0x4, 0x1e);
	write_register_all(0x5, 0x11);
	write_register_all(0x6, 0x11);
	write_register_all(0x7, 0x1e);
	write_register_all(0x8, 0x00);
	sleep_ms(1000);
	//C
	write_register_all(0x1, 0x0e);
	write_register_all(0x2, 0x11);
	write_register_all(0x3, 0x10);
	write_register_all(0x4, 0x10);
	write_register_all(0x5, 0x10);
	write_register_all(0x6, 0x11);
	write_register_all(0x7, 0x0e);
	write_register_all(0x8, 0x00);
	sleep_ms(1000);
	//D
	write_register_all(0x1, 0x1e);
	write_register_all(0x2, 0x11);
	write_register_all(0x3, 0x11);
	write_register_all(0x4, 0x11);
	write_register_all(0x5, 0x11);
	write_register_all(0x6, 0x11);
	write_register_all(0x7, 0x1e);
	write_register_all(0x8, 0x00);
	sleep_ms(1000);
	//E
	write_register_all(0x1, 0x1f);
	write_register_all(0x2, 0x10);
	write_register_all(0x3, 0x10);
	write_register_all(0x4, 0x1f);
	write_register_all(0x5, 0x10);
	write_register_all(0x6, 0x10);
	write_register_all(0x7, 0x1f);
	write_register_all(0x8, 0x00);
	sleep_ms(1000);
	//F
	write_register_all(0x1, 0x0f);
	write_register_all(0x2, 0x10);
	write_register_all(0x3, 0x10);
	write_register_all(0x4, 0x1f);
	write_register_all(0x5, 0x10);
	write_register_all(0x6, 0x10);
	write_register_all(0x7, 0x10);
	write_register_all(0x8, 0x00);
}