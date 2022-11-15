### TODO:

Create a 'sequencer' that allows you to record BOOT button presses and play them on the Neopixel, and also play a sequence of read/write commands. You should be able to:
- record at a least a few seconds of button input to your RP2040 (in RAM)
- replay a recorded sequence on your NeoPixel
- loop a recording
- save a recording to your laptop (the Python Serial library is one way to do this)
- play a recording from your laptop
- record 'macros' (a sequence of console commands) based on keystrokes in your serial console
- hand-edit a list of register read/write commands on your laptop, and play them on the RP2040
- include multiple I/O sources in a recording, and remap among the following:
    - inputs: BOOT button, console commands, register read/write commands
    - outputs: neopixel color, neopixel brightness, data over serial, register read/write commands

## Code
```
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
    //printf("button_is_pressed:  0x%08x\n",   status.button_is_pressed);
    //printf("light_color:        0x%08x\n",   status.light_color);
}  

// void neopixel_init() {
//     // power up WS2812 on Qt Py, using pin defs from board header file
//     // pico-sdk/src/boards/include/boards/adafruit_qtpy_rp2040.h
//     gpio_init(POWER_PIN);
//     gpio_set_dir(POWER_PIN, GPIO_OUT);
//     gpio_put(POWER_PIN, 1);

//     // program PIO using helper function from ws2812.pio
//     uint offset = pio_add_program(PIO, &ws2812_program);
//     ws2812_program_init(PIO, SM, offset, PIN, FREQ, IS_RGBW);
// }
           
// void neopixel_set_rgb(uint32_t rgb) {
//     // convert RGB to GRB
//     uint32_t grb = ((rgb & 0xFF0000) >> 8) | ((rgb & 0x00FF00) << 8) | (rgb & 0x0000FF);
//     pio_sm_put_blocking(pio0, 0, grb << 8u);
// }

int main() {

    stdio_init_all();
    gpio_init(QTPY_BOOT_PIN);
    gpio_set_dir(QTPY_BOOT_PIN, GPIO_IN);
    neopixel_init();

    Flashlight status;
    status.last_serial_byte =  0x00000000;
    status.button_is_pressed = 0x00000000;
    status.light_color =       0x00ff0000;

    int i = 0;
    int j = 0;
    uint32_t arr[1000];
    char input;
    scanf("%c",&input);
    printf("Start to press button.\n");
    while (i < 1000) {
        if (gpio_get(QTPY_BOOT_PIN)) { // poll every cycle, 0 = "pressed"
            status.button_is_pressed = 0x00000000;
            arr[i] = 0x00000000;
        } else {
            status.button_is_pressed = 0x00000001;
            arr[i] = 0x00000001;
        }
        if (status.button_is_pressed) { // poll every cycle
            neopixel_set_rgb(status.light_color);
        } else {
            neopixel_set_rgb(0x00000000);
        }
        render_to_console(status);
        sleep_ms(10); // don't DDOS the serial console
        i += 1;
        printf("%d\n",i*10);
    }
    printf("Start Replay!");
    while(true){
        while(j < 1000){
            if(arr[j]){
                neopixel_set_rgb(status.light_color);
            }else{
                neopixel_set_rgb(0x00000000);
            }
            render_to_console(status);
            sleep_ms(10); // don't DDOS the serial console
            j += 1;
        }
        j = 0;
    }
    return 0;
}
```
## Result
The QT py 2040 can record the frequency of button press and replay the process. https://youtu.be/4BjdElhO678


