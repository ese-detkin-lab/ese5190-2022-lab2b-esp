#include <stdio.h>

#include <stdlib.h>

#include "pico/stdlib.h"

#include "hardware/pio.h"

#include "pio_i2c.h"

#define PIN_SDA 22
#define PIN_SCL 23
const int addr = 0x39;
uint8_t clear_low = 0x94;
uint8_t clear_high = 0x95;
const uint8_t red_low = 0x96;
const uint8_t red_high = 0x97;
const uint8_t green_low = 0x98;
const uint8_t green_high = 0x99;
const uint8_t blue_low = 0x9A;
const uint8_t blue_high = 0x9B;
uint8_t PDATA = 0x9C;
uint8_t ATIME = 0x81;


const uint8_t ALS_TIME = 219;
const uint8_t pon_addr = 0x80;
const uint8_t pon_val = 0b00000111;

const uint8_t gmode_addr = 0xAB;
const uint8_t gmode_val = 0b00000001;

const uint8_t sai_addr = 0x9F;
const uint8_t sai_val = 0b00000000;

uint16_t buffer_c;
uint16_t buffer_r;
uint16_t buffer_g;
uint16_t buffer_b;

uint8_t buf[2];

int returned;


PIO pio = pio0;
uint sm = 0;

bool reserved_addr(uint8_t addr) {
    return (addr & 0x78) == 0 || (addr & 0x78) == 0x78;
}

int apds_init() {

    buf[0] = pon_addr;
    buf[1] = pon_val;
    returned = pio_i2c_write_blocking(pio, sm, addr, buf, 2); // true to keep master control of bus
    if (returned ==  PICO_ERROR_GENERIC) {
        printf("ERROR: PON not written\n");
        return -1;
    }


    buf[0] = ATIME;
    buf[1] = ALS_TIME;
    pio_i2c_write_blocking(pio, sm, addr, buf, 2);

    // // config gmode
    // //printf("wite to gmode\n");
    // buf[0] = gmode_addr;
    // buf[1] = gmode_val;
    // returned = pio_i2c_write_blocking(pio, sm, addr, buf, 2); // true to keep master control of bus
    // if (returned ==  PICO_ERROR_GENERIC) {
    //     printf("ERROR: GMODE not written\n");
    //     return -2;
    // }

    return 1;
    
}

static void read_raw() {
    uint8_t lsb;
    uint8_t msb;
    uint8_t proxi;

    // printf("read from buffer-c\n");
    pio_i2c_write_blocking(pio, sm, addr, &PDATA, 1);
    pio_i2c_read_blocking(pio, sm, addr, &proxi, 1);


    pio_i2c_write_blocking(pio, sm, addr, &clear_low ,1);
    pio_i2c_read_blocking(pio, sm, addr, &lsb, 1);


    pio_i2c_write_blocking(pio, sm, addr, &clear_high, 1);
    pio_i2c_read_blocking(pio, sm, addr, &msb, 1);

    printf("distance: %u\n", proxi);
    printf("msb: %u\n", msb);
    printf("lsb: %u\n", lsb);

    buffer_c = (msb << 8) | lsb;
    //i2c_read_blocking(i2c_default, clear_high, (buffer_c+4), 8, true);

    // printf("read from buffer-r\n");
    // i2c_read_blocking(i2c_default, red_low, buffer_r, 8, true);
    // i2c_read_blocking(i2c_default, red_high, (buffer_r+4), 8, true);

    // printf("read from buffer_g\n");
    // i2c_read_blocking(i2c_default, green_low, buffer_g, 8, true);
    // i2c_read_blocking(i2c_default, green_high, (buffer_g+4), 8, true);

    // printf("read from buffer_b\n");
    // i2c_read_blocking(i2c_default, blue_low, buffer_b, 8, true);
    // i2c_read_blocking(i2c_default, blue_high, (buffer_b+4), 8, true);
    // buf[0] = sai_addr;
    // buf[1] = sai_val;

    // pio_i2c_write_blocking(pio,sm, addr, buf, 2);

    //}
}



int main() {

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
            result = pio_i2c_read_blocking(pio, sm, addr, NULL, 0);

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
    

    while (returned > 0) {
        // printf("entered while\n");
        read_raw();
        // printf("back from get_raw\n");
        // print_buffer(buffer_c);
        // print_buffer(buffer_r);
        // print_buffer(buffer_g);
        // print_buffer(buffer_b);
        printf("clear: %u\n", buffer_c);

        // printf("red: %u\n", buffer_r);
        // printf("green: %u\n", buffer_g);
        // printf("blue: %u\n", buffer_b);
        sleep_ms(100);
    }

    return 0;
}

//                  cd pico/pico-examples/build/pio_sequencer
