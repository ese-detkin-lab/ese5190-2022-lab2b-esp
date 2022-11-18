#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "ws2812.pio.h" // pio0

typedef          uint32_t   VALUE;
typedef volatile uint32_t * ADDRESS;

VALUE register_read(ADDRESS address) {
    return *address;
}

void register_write(ADDRESS address, VALUE value) {
    *address = value;
}

#define BOOT_PIN 21

#define PIO			pio0
#define SM			0
#define FREQ		800000
#define WS2812_PIN	12
#define POW_PIN		11
#define IS_RGBW		true

void neopixel_set_rgb(uint32_t rgb) {
    // convert RGB to GRB
    uint32_t grb = ((rgb & 0xFF0000) >> 8) | ((rgb & 0x00FF00) << 8) | (rgb & 0x0000FF);
    pio_sm_put_blocking(PIO, 0, grb << 8u);
}

#define ADDRESS_LENGTH 8 //32 bits address
#define MSG_LENGTH 8 //32 bits input

// initialize buffer
char addr[10] = {'0'}; // store 8 bytes hex number = 32 bits binary number
char msg[9] = {'0'}; // store 8 bytes hex number = 32 bits binary number
ADDRESS address;
VALUE value = 0;
uint32_t hex_addr; // addr convertor
uint32_t hex_msg = 0; // msg convertor

typedef struct {
    uint32_t reg_address;
    uint32_t reg_value;
} Register_status; 

void render_to_console(Register_status status) {
    // adjust console window height to match 'frame size'

    printf("reg_address:  0x%08x\n",   status.reg_address);
    printf("reg_value:    0x%08x\n",   status.reg_value);
    for (uint8_t i=0; i<6; i++) { 
        printf("\n");
    }
}  

void access_register(char a[10]){
    uint idx = 0;
    printf("What register would you like to access?\n");
    while(1){
        char c = getchar_timeout_us(200);
        if (c == 0x0D){
            //a[idx] = 0;
            printf("\n");
            return;
        }
        if ((c >= 0x20) && (c <= 0x7E)){
            printf("%c", c);
            if (idx < 10){
                a[idx++] = c;
            }
        }
    }

}

void input_value(char b[8]){
    uint idx = 0;
    printf("What value would you like to input?\n");
    while(1){
        char c = getchar_timeout_us(200);
        if (c == 0x0D){
            printf("\n");
            return;
        }
        if ((c >= 0x20) && (c <= 0x7E)){
            printf("%c", c);
            if (idx < 8){
                b[idx++] = c;
            }
        }
    }
}

void repl_read(){
    Register_status status;
    access_register(addr);
    int flag = sscanf(addr, "%x", &hex_addr);
        if(flag == 1){
            printf("Sscanf status: %d\n", flag);
            printf("The hex address is: %x\n", hex_addr);
            address = (ADDRESS) hex_addr;
            status.reg_address = hex_addr;
        }else{
            printf("sscanf faild");
        }
    
    value = register_read(address);
    status.reg_value = value;
    render_to_console(status);

}

void repl_write(){
    Register_status status;
    access_register(addr);
    int flag = sscanf(addr, "%x", &hex_addr);
        if(flag == 1){
            printf("Sscanf status: %d\n", flag);
            printf("The hex address is: %x\n", hex_addr);
            address = (ADDRESS) hex_addr;
            status.reg_address = hex_addr;
        }else{
            printf("sscanf faild");
        }
    input_value(msg);
    hex_msg = (VALUE)strtol(msg, NULL, 16);
    printf("Start write:\n");
    register_write(address,hex_msg);
    status.reg_value = hex_msg;
    render_to_console(status);
}

int main() {

    stdio_init_all();
    gpio_init(BOOT_PIN);
    gpio_set_dir(BOOT_PIN, GPIO_IN);
	
    // neopixel initialisation
	gpio_init(POW_PIN);
    gpio_set_dir(POW_PIN, GPIO_OUT);
    gpio_put(POW_PIN,1);	
	uint offset = pio_add_program(PIO, &ws2812_program);
    ws2812_program_init(PIO, SM, offset, WS2812_PIN, FREQ, IS_RGBW);

    char cmd =  'r';
    int boot_prss = 0;
    uint32_t neo =   0x00ffffff; // Blue for button pressed
    char flag = '0';

    while (true) {
        cmd = getchar_timeout_us(0);
        switch(cmd) { // keep checking
            case 'r':
                printf("Read\n");
                neopixel_set_rgb(0x0000ff00); // Green for read
                repl_read();
                break;
            case 'w':
                printf("Write\n");
                neopixel_set_rgb(0x00ff0000); // Red for write
                repl_write();
                break;
            case 'd':
                printf("Record\n");
                while (true){
                    flag = 0x00000000;
                    flag = getchar_timeout_us(0);
                    if (gpio_get(BOOT_PIN)) { // keep checking, 0 means pressed
                        boot_prss = 0;
                        neo = 0x00ffffff;
                    } else {
                        boot_prss = 1;
                    }
                    if (boot_prss) { // keep checking
                        neopixel_set_rgb(neo);
                    } else {
                        neopixel_set_rgb(0x00000000);
                    }
                    if(flag=='N'){
                        break;
                    }
                    sleep_ms(10);
                }
                break;
            case 'y':
                printf("Replay\n");
				while(true){
                    flag = '0';
                    flag = getchar_timeout_us(0);
                    if(flag == 'p'){
                        neopixel_set_rgb(0x00ffffff);
                    }
                    if(flag == 'n'){
                        neopixel_set_rgb(0x00000000);
                    }
                    if (flag == 'N') {
                        break;
                    }
                    sleep_ms(10); 
                }
                break;
        }
        if (gpio_get(BOOT_PIN)) { // keep checking, 0 means pressed
            boot_prss = 0;
            neo = 0x00ffffff;
        } else {
            boot_prss = 1;
        }
        if (boot_prss) { // keep checking
            neopixel_set_rgb(neo);
        } else {
            neopixel_set_rgb(0x00000000);
        }
        sleep_ms(10);
    }
    return 0;
}

