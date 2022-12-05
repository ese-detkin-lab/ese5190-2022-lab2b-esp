#include "pico/stdlib.h"
#include <stdio.h>
#include "neopixel.h"
#include "hardware/gpio.h"
#include "ws2812.pio.h"
#include "hardware/pio.h"
#define PIO         pio0
#define SM          0
#define FREQ        800000
#define PIN         12
#define POWER_PIN   11
#define IS_RGBW     true  

#define QTPY_BOOT_PIN 21

//sio base 0xd0000000
//offset 0x00000004

volatile uint32_t * boot_pin_adress;
uint32_t full_gpio_register_value;
uint32_t pin21_selection_mask;
uint32_t selected_pin_state;
uint32_t shifted_pin_21_state;

typedef struct {
    uint32_t last_serial_byte;
    uint32_t button_is_pressed;
    uint32_t light_color;
} Flashlight; 

void render_to_console(Flashlight status) {
    // adjust console window height to match 'frame size'
    //for (uint8_t i=0; i<10; i++) { 
    //    printf("\n");
    //}
    printf("0x%08x\n",   status.button_is_pressed);
    //printf("light_color:        0x%08x\n",   status.light_color);
}   

int main() {

    stdio_init_all();
    gpio_init(QTPY_BOOT_PIN);
    gpio_set_dir(QTPY_BOOT_PIN, GPIO_IN);
    neopixel_init();

    Flashlight status;
    status.last_serial_byte =  0x00000000;
    status.button_is_pressed = 0x00000000;
    status.light_color =       0x000000ff; 

    while(stdio_usb_connected()!=true);
    int max = 1000;
    int i = 0;
    int j = 0;
    uint32_t button_log[10*100];
    int user_input_speed;
    int speed_ms_delay;

    printf("Recording\n");
    while (i < max) {
        if (gpio_get(QTPY_BOOT_PIN)) { // poll every cycle, 0 = "pressed"
            status.button_is_pressed = 0x00000000;
            button_log[i] = 0x00000000;
        } else {
            status.button_is_pressed = 0x00000001;
            button_log[i] = 0x00000001;
        }
        if (status.button_is_pressed) { // poll every cycle
            neopixel_set_rgb(status.light_color);
        } else {
            neopixel_set_rgb(0x00000000);
        }
        render_to_console(status);
        sleep_ms(10); // don't DDOS the serial console
        i += 1;
    }
    sleep_ms(1000);
    printf("Enter a percentage from 1-500 to replay the sequence at a speed of:");
    scanf("%d", &user_input_speed);
    speed_ms_delay = 1000 / user_input_speed;

    while(true){
        while(j < max){
            if(button_log[j]){
                neopixel_set_rgb(status.light_color);
            }else{
                neopixel_set_rgb(0x00000000);
            }
            //render_to_console(status);
            printf("%d\n", button_log[j]);
            
            sleep_ms(speed_ms_delay); // don't DDOS the serial console
            j += 1;
        }
        j = 0;
    }
    return 0;
}