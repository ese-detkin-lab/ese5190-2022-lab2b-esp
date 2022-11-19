### TODO:

Update your sequencer to be able to slow down and speed up recordings/replay. On the input side, the goal is ultimately to be able to handle the full 4 Gbps "firehose" from the PIO logic analyzer example in the SDK manual, which would fill up 256Kb of RAM in only 31 µs at a system clock speed of 125000 MHz if captured 'raw'! On the output side, the goal is to be able to output precisely timed sequences from the PIO at system clock resolution based on a handful of control points.

- update your sequencer to be able to record just the timestamped transitions between input values, and to be able to play data in this format
- give your sequencer the ability to select a range of output rates for both live and recorded input

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

int main() {

    stdio_init_all();
    gpio_init(QTPY_BOOT_PIN);
    gpio_set_dir(QTPY_BOOT_PIN, GPIO_IN);
    neopixel_init();

    Flashlight status;
    status.last_serial_byte =  0x00000000;
    status.button_is_pressed = 0x00000000;
    status.light_color =       0x00ff0000;

    int max = 10*1000;
    int i = 0;
    int j = 0;
    int speed=1;
    uint32_t arr[10*100];
    char input;
    scanf("%c",&input);
    printf("Start to press button.\n");
    while (i < max/10) {
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
    }
    while(true){
        printf("Enter 1 to replay in normal speed. Enter 2 to slow down replay. Enter 3 to speed up replay.\n");
        scanf("%d",&speed);
        if(speed == 3){
            printf("Start Replay fast!\n");
            speed = 5;
        }else if(speed == 1){
            printf("Start Replay in normal speed!\n");
            speed = 10;
        }else{
            printf("Start Replay slow!\n");
            speed = 20;
        }
        while(j < max/10){
            if(arr[j]){
                neopixel_set_rgb(status.light_color);
            }else{
                neopixel_set_rgb(0x00000000);
            }
            render_to_console(status);
            sleep_ms(speed); // don't DDOS the serial console
            j += 1;
        }
        j = 0;
    }
    return 0;
}
```
Python code
```
# -*- coding: utf-8 -*-
import serial
import time
import io


def file_to_list(file_path, data_list):
    with open(file_path, 'rb+') as f:
        data = f.readline()
        while data:
            data_list.append(data.decode().replace('\n', ''))
            data = f.readline()
    f.close()
    return data_list


def slowdown(data_list, data_new, rate):
    for data in data_list:
        for i in range(rate):
            data_new.append(data)
    return data_new


def speedup(data_list, data_new, rate):
    for i in range(len(data_list)):
        if i % rate == 0:
            data_new.append(data_list[i])
    return data_new



if __name__ == "__main__":
    print("init")
    # pressed = b'p'
    # no_press = b'n'
    # Write the corresponding test file to the folder one level above the current directory
    path = './test.txt'
    try:
        qtpy = serial.Serial('COM7', 9600)
        print(qtpy.name)
        sio = io.TextIOWrapper(io.BufferedRWPair(qtpy, qtpy))
    except Exception as e:
        print("COM can not open,Exception=", e)

    while True:
        print("Choose the mode: R for record and P for replay")
        command = input()
        if command == 'R':
            data_in = []
            print("Start the recording after red light. Recording time is 10s:")
            qtpy.write(b'r')
            print("Reading from qtpy...")
            start = time.time()
            while time.time() - start < 5:
                line = qtpy.readline()
                line = line.decode().replace(' ', '\n')
                data_in.append(line)
            qtpy.write(b'N')
            print("Finish the recording")
            with open(path, 'wb+') as f:
                for item in data_in:
                    f.write(item.encode('utf-8'))
                f.close()

        if command == 'P':
            data_out = []
            data_final = []
            file_to_list(path, data_out)
            print("Now play the recording:")
            print("Choose the speed mode: normal/SD/SU")
            ans = input()
            print("Input the int rate you want to speed up/down")
            rate = int(input())
            if ans == 'normal':
                data_final = data_out
            if ans == 'SD':
                data_final = slowdown(data_out, data_final,rate)
            if ans == 'SU':
                data_final = speedup(data_out, data_final,rate)
           
            print("Choose the times you want to replay record:")
            t = input()
            t = int(t)
            qtpy.write(b'p')
            for i in range(t):
                for item in data_final:
                    if item == '1\r':
                        qtpy.write(b'1')
                    if item == '0\r':
                        qtpy.write(b'0')
            qtpy.write(b'N')
            print("Finish replay")
```


## Result
The txt file can 
It can record the frequency of the button. It also can replay, speed up and slow down the recorded frequency. https://youtube.com/shorts/Lp8wNPlwWVQ
