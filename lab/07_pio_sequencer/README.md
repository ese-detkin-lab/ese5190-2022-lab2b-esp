### TODO:

- modify your sequencer to use the PIO as its primary I/O engine, including the ability to R/W any register 
## Code
Modify the code in part 4 to use the PIO to R/W
```
#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "ws2812.h"

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "ws2812.pio.h"

#include "hardware/dma.h"
#include "hardware/structs/bus_ctrl.h"

#include <string.h>
#include "pico/binary_info.h"
#include "hardware/i2c.h"
#include "registers.h"

#define IS_RGBW true

#define WS2812_PIN 12
#define WS2812_POWER_PIN 11
#define FREQ 800000

#define record 'r'
#define replay 'p'
#define BOOT_PIN 21

const uint CAPTURE_PIN_BASE = 21;
const uint CAPTURE_PIN_COUNT = 1;
const uint CAPTURE_N_SAMPLES = 1;
const uint TRIGGER_PIN = 21;

int pressed = 0;

static inline uint bits_packed_per_word(uint pin_count) {
    const uint SHIFT_REG_WIDTH = 32;
    return SHIFT_REG_WIDTH - (SHIFT_REG_WIDTH % pin_count);
}

void logic_analyser_init(PIO pio, uint sm, uint pin_base, uint pin_count, float div) {
    // Load a program to capture n pins. This is just a single `in pins, n`
    // instruction with a wrap.
    uint16_t capture_prog_instr = pio_encode_in(pio_pins, pin_count);
    struct pio_program capture_prog = {
            .instructions = &capture_prog_instr,
            .length = 1,
            .origin = -1
    };
    uint offset = pio_add_program(pio, &capture_prog);
    pio_sm_config c = pio_get_default_sm_config();
    sm_config_set_in_pins(&c, pin_base);
    sm_config_set_wrap(&c, offset, offset);
    sm_config_set_clkdiv(&c, div);
    sm_config_set_in_shift(&c, true, true, bits_packed_per_word(pin_count));
    sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_RX);
    pio_sm_init(pio, sm, offset, &c);
}

void logic_analyser_arm(PIO pio, uint sm, uint dma_chan, uint32_t *capture_buf, size_t capture_size_words,
                        uint trigger_pin, bool trigger_level) {
    pio_sm_set_enabled(pio, sm, false);
    pio_sm_clear_fifos(pio, sm);
    pio_sm_restart(pio, sm);

    dma_channel_config c = dma_channel_get_default_config(dma_chan);
    channel_config_set_read_increment(&c, false);
    channel_config_set_write_increment(&c, true);
    channel_config_set_dreq(&c, pio_get_dreq(pio, sm, false));

    dma_channel_configure(dma_chan, &c,
        capture_buf,        // Destination pointer
        &pio->rxf[sm],      // Source pointer
        capture_size_words, // Number of transfers
        true                // Start immediately
    );

    pio_sm_exec(pio, sm, pio_encode_wait_gpio(trigger_level, trigger_pin));
    pio_sm_set_enabled(pio, sm, true);
}

void print_capture_buf(const uint32_t *buf, uint pin_base, uint pin_count, uint32_t n_samples) {
    printf("Capture:\n");
    uint record_size_bits = bits_packed_per_word(pin_count);
    for (int pin = 0; pin < pin_count; ++pin) {
        printf("%02d: ", pin + pin_base);
        for (int sample = 0; sample < n_samples; ++sample) {
            uint bit_index = pin + sample * pin_count;
            uint word_index = bit_index / record_size_bits;
       
            if(buf[word_index]) {
                pressed = 0;
            } else {
                pressed = 1;
            }
        }
        // printf("\n");
    }
}



int main() {

    PIO pio_0 = pio0;
    PIO pio_1 = pio1;
    uint sm = 0;
    uint dma_chan = 0;

    stdio_init_all();
    uint offset = pio_add_program(pio_0, &ws2812_program);
    //rp_init();
    ws2812_program_init(pio_0, sm, offset, WS2812_PIN, FREQ, IS_RGBW);
    turn_on_pixel();

    gpio_init(BOOT_PIN);
    gpio_set_dir(BOOT_PIN, GPIO_IN);

    uint32_t key = 0x00000000;
    uint32_t flag = 0x00000000;

    uint total_sample_bits = CAPTURE_N_SAMPLES * CAPTURE_PIN_COUNT;
    total_sample_bits += bits_packed_per_word(CAPTURE_PIN_COUNT) - 1;
    uint buf_size_words = total_sample_bits / bits_packed_per_word(CAPTURE_PIN_COUNT);
    uint32_t *capture_buf = malloc(buf_size_words * sizeof(uint32_t));
    hard_assert(capture_buf);

    bus_ctrl_hw->priority = BUSCTRL_BUS_PRIORITY_DMA_W_BITS | BUSCTRL_BUS_PRIORITY_DMA_R_BITS;

    logic_analyser_init(pio_1, sm, CAPTURE_PIN_BASE, CAPTURE_PIN_COUNT, 256.f);

    while(true){
        key = getchar_timeout_us(0);
        switch(key){
            case 'r':
                set_pixel_color(0X00FF0000);
                sleep_ms(1000);
                while(true){
                    flag = 0x00000000;
                    flag = getchar_timeout_us(0);
                    logic_analyser_arm(pio_1, sm, dma_chan, capture_buf, buf_size_words, CAPTURE_PIN_BASE, false);
                    print_capture_buf(capture_buf, CAPTURE_PIN_BASE, CAPTURE_PIN_COUNT, CAPTURE_N_SAMPLES);
                    if(pressed) {
                        printf("1\n");
                        set_pixel_color(0X0000FF00);
                    } 
                    else {
                        printf("0\n");
                        set_pixel_color(0x00000000);
                    }
                    if(flag == 'N'){
                        set_pixel_color(0X00000000);
                        sleep_ms(10);
                        break;
                    }
                    sleep_ms(10); 
                }
                break;
            
            case 'p':
                while(true){
                    flag = 0x00000000;
                    flag = getchar_timeout_us(0);
                    if(flag == '1'){
                        set_pixel_color(0X000000FF);
                    }
                    if(flag == '0'){
                        set_pixel_color(0x00000000);
                    }
                    if(flag == 'N'){
                        set_pixel_color(0x00000000);
                        sleep_ms(10);
                        break;
                    }
                    sleep_ms(10);
                }
                break;

        }
    }
}
```
Python Code
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
The result is same as part 4
