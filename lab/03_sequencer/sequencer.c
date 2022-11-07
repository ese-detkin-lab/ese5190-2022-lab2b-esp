#include "pico/stdlib.h"
#include <stdio.h>
#include "neopixel.h"
#include "hardware/gpio.h"
// #include "registers.h"
#include <time.h>
#include <string.h>

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
int inputs;


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
    int count;
    int flag = 1;
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
        if (gpio_get(QTPY_BOOT_PIN)){
            printf("%d, %d\r\n", count,0);
            count += 1;
        } else {
            printf("%d, %d\r\n", count,1);
            count+=1;
        }
    }
    }
    return 1;

}

int take_yn() {
    int yn_input;
    sleep_ms(5000);
    yn_input = getchar_timeout_us(0);
    printf("you inputted: %u\n", yn_input);
    if (yn_input == '1'){
        return 1;
    }
    else if (yn_input == '0'){
        return 0;
    }
    else {
        printf("Invalid input, please  enter again!\n");
        take_yn();
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
    int flagg = 0;
    while (flagg== 0) {
        //record for 10s
        // printf("Please input sequence using the boot button");

        int time_return = check_time(10000);
        if (time_return == 1){
            printf("reached\n");
            flagg = 1;
        }
        // printf("Do you want to input again? 1 for yes, 0 for no\n");
        // flagg = take_yn();
        // if (flagg == 0){
        //     break;
        // }

    }
    return 0;
}   