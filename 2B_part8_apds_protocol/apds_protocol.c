#include <stdio.h>

#include <stdlib.h>

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "neopixel.h"
#include "pio_i2c.h"

#define PIN_SDA 22
#define PIN_SCL 23
#define WS2812_POWER_PIN 11
#define WS2812_PIN 12
const int addr = 0x39;
uint8_t clear_low = 0x94;
uint8_t clear_high = 0x95;
uint8_t red_low = 0x96;
uint8_t red_high = 0x97;
const uint8_t green_low = 0x98;
const uint8_t green_high = 0x99;
const uint8_t blue_low = 0x9A;
const uint8_t blue_high = 0x9B;
uint8_t PDATA = 0x9c;
uint8_t ATIME = 0x81;


const uint8_t ALS_TIME = 219;
const uint8_t enable_addr = 0x80;
// const uint8_t pon_val = 0b00000111;

const uint8_t gmode_addr = 0xAB;
const uint8_t gmode_val = 0b00000000;

// const uint8_t sai_addr = 0x9F;
// const uint8_t sai_val = 0b00000000;


#define GEN 0 //gesture enable 
#define AEN 1 //ALS Enable. 
#define PON 1 //Power ON. 
#define PEN 0 //Proximity enable.

#define CONFIGURATION (GEN << 6U) | (PEN << 2u) | (AEN << 1u) | PON

uint16_t buffer_c;
uint16_t buffer_r;
uint16_t buffer_g;
uint16_t buffer_b;

uint8_t buf[2];

int returned;


PIO pio = pio0;
uint sm = 1;

bool reserved_addr(uint8_t addr) {
    return (addr & 0x78) == 0 || (addr & 0x78) == 0x78;
}


int apds_init() {

    buf[0] = enable_addr;
    buf[1] = CONFIGURATION;
    returned = pio_i2c_write_blocking(pio, sm, addr, buf, 2, false); // true to keep master control of bus
    if (returned ==  PICO_ERROR_GENERIC) {
        printf("ERROR: PON not written\n");
        return -1;
    }


    buf[0] = ATIME;
    buf[1] = ALS_TIME;
    pio_i2c_write_blocking(pio, sm, addr, buf, 2, false);

    // config gmode
    // //printf("wite to gmode\n");
    // buf[0] = gmode_addr;
    // buf[1] = gmode_val;  //gmode = 0 at first
    // returned = pio_i2c_write_blocking(pio, sm, addr, buf, 2,false); // true to keep master control of bus
    // if (returned ==  PICO_ERROR_GENERIC) {
    //     printf("ERROR: GMODE not written\n");
    //     return -2;
    // }

    return 1;
    
}

static void read_prox() {
    uint8_t proxi;

    printf("read from buffer-c\n");
    pio_i2c_write_blocking(pio, sm, addr, &PDATA, 1,true);
    pio_i2c_read_blocking(pio, sm, addr, &proxi, 1, false);

    printf("distance: %u\n", proxi);
   
}

void read_rgbc(int32_t* r, int32_t* g, int32_t* b, int32_t* c) {

    uint8_t buf[8];
    uint8_t reg = clear_low;
    returned = pio_i2c_write_blocking(pio, sm, addr, &reg, 1, true);  // true to keep master control of bus
    // printf("write rgbc status: %d\n", returned);
    returned = pio_i2c_read_blocking(pio, sm, addr, buf, 8, false);  // false - finished with bus
    // printf("read rgbc status: %d\n", returned);


    *c = (buf[1] << 8) | buf[0];
    *r = (buf[3] << 8) | buf[2];
    *g = (buf[5] << 8) | buf[4];
    *b = (buf[7] << 8) | buf[6];
}

static inline uint32_t rgbc_u32(uint8_t r, uint8_t g, uint8_t b) {
    return
            ((uint32_t) (r) << 8) |
            ((uint32_t) (g) << 16) |
            (uint32_t) (b);
}

int main() {
    int32_t r,g,b,c;
    uint32_t color;

    stdio_init_all();

    while(stdio_usb_connected()!=true);
    sleep_ms(2000);

    uint offset = pio_add_program(pio, &i2c_program);
    i2c_program_init(pio, sm, offset, PIN_SDA, PIN_SCL);
 
    printf("\nPIO I2C Bus Scan\n");
    printf("   0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\n");

    for (int addr = 0; addr < (1 << 7); ++addr) {
        if (addr % 16 == 0) {
            printf("%02x ", addr);
        }
        // Perform a 0-byte read from the probe address. The read function
        // returns a negative result NAK'd any time other than the last data
        // byte. Skip over reserved addresses.
        int result;
        if (reserved_addr(addr))
            result = -1;
        else
            result = pio_i2c_read_blocking(pio, sm, addr, NULL, 0, false);

        printf(result < 0 ? "." : "@");
        printf(addr % 16 == 15 ? "\n" : "  ");
    }
    printf("Done.\n");
    // Finished bus scan
   
    while(1){
        returned = apds_init();
        if(returned <0){
         printf("cannot initialize ADPS-9960.");
         }
         else{  // only after successfully initialize sensor, go to next step
            break;
         }
    }
    

    neopixel_init();
    while (returned >= 0) {
        // printf("entered while\n");
        // read_raw();
        // printf("back from get_raw\n");
        // print_buffer
        read_rgbc(&r, &g, &b, &c);
        printf("r:%d, g:%d, b:%d, c:%d\n", r, g, b, c);

        color = rgbc_u32((uint8_t)r, (uint8_t)g,(uint8_t)b);

        gpio_put(WS2812_POWER_PIN, true);
        gpio_put(WS2812_PIN, true);
        neopixel_set_rgb(color);
        sleep_ms(3000);
    }

    return 0;
}

//                  cd pico/pico-examples/build/pio_sequencer
