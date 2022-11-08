#include "pico/stdlib.h"
#include <stdio.h>
#include "neopixel.h"
#include "hardware/gpio.h"
// #include "registers.h"
#include <time.h>
#include <string.h>
#define WS2812_PIN 12
#define WS2812_POWER_PIN 11

#define QTPY_BOOT_PIN 21
#define SIO
// sio base is 
volatile uint32_t * boot_pin_address;
uint32_t full_gpio_register_value;
uint32_t pin_21_selection_mask;
uint32_t selected_pin_state;
uint32_t shifted_pin_21_state;

int t_start;
int t_end;
int t_track;
int input;
int answer; //store user input for mode 
int led_status = 1;


typedef struct {
    uint32_t last_serial_byte;
    uint32_t button_is_pressed;
    uint32_t light_color;
} sequencer; 

void render_to_console(sequencer status) {
    // adjust console window height to match 'frame size'
    for (uint8_t i=0; i<10; i++) { 
        printf("\n");
    }
    printf("button_is_pressed:  0x%08x\n",   status.button_is_pressed);
    printf("light_color:        0x%08x\n",   status.light_color);
}  

int check_time(int duration){
    int lastinput = 0;
    int count = 0;
    int flag = 1;
    int currentinput;
    absolute_time_t absolute_t_start = get_absolute_time();
    t_start = to_ms_since_boot(absolute_t_start);
    t_end = to_ms_since_boot(delayed_by_ms(absolute_t_start, duration));
    while (flag == 1){
        t_track = to_ms_since_boot(get_absolute_time());
        if (t_track >= t_end){
            flag = 0;
            break;
        }
        else {
            // printf("Not yet\n");
            currentinput = gpio_get(QTPY_BOOT_PIN);
            count += 1;

            // printf("Current: %u; last: %u", currentinput, lastinput);
            if (currentinput != lastinput) {
                printf("%d, %d\r\n", count,currentinput); //only when input change between 0 and 1, print CURRENT input
                lastinput = currentinput;
            }
            continue;
        }
    } 
     return 1;  
}

int take_yn() {
    int yn_input;
    yn_input = getchar_timeout_us(50);
    // printf("you inputted: %u\n", yn_input);
    if (yn_input == '1'){
        return 1;
    }
    else if (yn_input == '2'){
        return 2;
    }
    else {
        return 9;

    }
}



int main() {

    stdio_init_all();
    while (!stdio_usb_connected()){
            printf(".");
            sleep_ms(500);
        };

    gpio_init(QTPY_BOOT_PIN);
    gpio_set_dir(QTPY_BOOT_PIN, GPIO_IN);
    neopixel_init();

    sequencer status;
    status.last_serial_byte =  0x00000000;
    status.button_is_pressed = 0x00000000;
    status.light_color =       0x00ff0000;
    // int flagg = 0;
    while (true) {
        //record for 10s
        // printf("Please input sequence using the boot button");
       answer = take_yn();
        if(answer == 1){
            //printf("entered mode 1\n");
            int time_return = check_time(8000);
            if (time_return == 1){
                printf("reached\n");
                // flagg = 1;
            }
        }
        if(answer == 2){
            sleep_ms(500);
            getchar_timeout_us(50); //erase the enter
            while (!stdio_usb_connected()){
                printf(".");
                sleep_ms(500);
             };
            neopixel_init();

            printf("Ready to start replay on LED...\n");
            while(true){
                //printf("in the while loop\n");
                led_status = getchar_timeout_us(50);
                if(led_status == '1') { // if button pushed, then light up LED
                    gpio_put(WS2812_POWER_PIN, true);
                    gpio_put(WS2812_PIN, true);
                    neopixel_set_rgb(0x00ff0000);  // if 1, red light
                    sleep_ms(1000);
                    } else { 
                    // printf("got something\n");
                    gpio_put(WS2812_POWER_PIN, true);
                    gpio_put(WS2812_PIN, true);
                    neopixel_set_rgb(0x0000ff00);  //if 0, green light
                    sleep_ms(1000);
                }
            }
        }
        if(answer == 0){
            break;
            }
        
 
        // printf("Do you want to input again? 1 for yes, 0 for no\n");
        // flagg = take_yn();
        // if (flagg == 0){
        //     break;
        // }

    }
    return 0;
}   