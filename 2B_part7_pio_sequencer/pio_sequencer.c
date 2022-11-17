/**
Part 6 and Part 7
*/

#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/dma.h"
#include "hardware/structs/bus_ctrl.h"
#include "neopixel.h"
#include "string.h"


#define WS2812_PIN 12
#define WS2812_POWER_PIN 11

// Some logic to analyse:
#include "hardware/structs/pwm.h"

const uint CAPTURE_PIN_BASE = 22;
const uint CAPTURE_PIN_COUNT = 2;
const uint CAPTURE_N_SAMPLES = 960;
const uint TRIGGER_PIN = 21;

volatile uint32_t previous_buf = 0;
uint32_t current_capture = 0;
//int results[100000] = {-1};
// int one = 1;
// int *ptr_one;    //indicate a change
// int zero = 0;
// int *ptr_zero;   // indicate no change


static inline uint bits_packed_per_word(uint pin_count) {
    // If the number of pins to be sampled divides the shift register size, we
    // can use the full SR and FIFO width, and push when the input shift count
    // exactly reaches 32. If not, we have to push earlier, so we use the FIFO
    // a little less efficiently.
    const uint SHIFT_REG_WIDTH = 32;
    return SHIFT_REG_WIDTH - (SHIFT_REG_WIDTH % pin_count);
}

void print_array(int arr[]) {
    for (int i = 0; i < 10; i ++ ){
        printf("%d", arr[i]);
    }
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

    // Configure state machine to loop over this `in` instruction forever,
    // with autopush enabled.
    pio_sm_config c = pio_get_default_sm_config();
    sm_config_set_in_pins(&c, pin_base);
    sm_config_set_wrap(&c, offset, offset);
    sm_config_set_clkdiv(&c, div);
    // Note that we may push at a < 32 bit threshold if pin_count does not
    // divide 32. We are using shift-to-right, so the sample data ends up
    // left-justified in the FIFO in this case, with some zeroes at the LSBs.
    sm_config_set_in_shift(&c, true, true, bits_packed_per_word(pin_count));
    sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_RX);
    pio_sm_init(pio, sm, offset, &c);
}



void logic_analyser_arm(PIO pio, uint sm, uint dma_chan, uint32_t *capture_buf, size_t capture_size_words,
                        uint trigger_pin, bool trigger_level) {
    pio_sm_set_enabled(pio, sm, false);
    // Need to clear _input shift counter_, as well as FIFO, because there may be
    // partial ISR contents left over from a previous run. sm_restart does this.
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

    //pio_sm_exec(pio, sm, pio_encode_wait_gpio(trigger_level, trigger_pin));
    pio_sm_set_enabled(pio, sm, true);
}

void now_print(int indi) {
    if (indi == 1){
        printf("-");
    } else {
        printf("_");
    }
}


void print_capture_buf(const uint32_t *buf, uint pin_base, uint pin_count, uint32_t n_samples, int count) {
    // Display the capture buffer in text form, like this:
    // 00: __--__--__--__--__--__--
    // 01: ____----____----____----
    printf("Capture:\n");
    // Each FIFO record may be only partially filled with bits, depending on
    // whether pin_count is a factor of 32.
    uint unchange_time = 0;
    uint record_size_bits = bits_packed_per_word(pin_count);
    for (int pin = 0; pin < pin_count; ++pin) {
        printf("%02d: ", pin + pin_base);
        for (int sample = 0; sample < n_samples; ++sample) {
            uint bit_index = pin + sample * pin_count;
            uint word_index = bit_index / record_size_bits;
            // Data is left-justified in each FIFO entry, hence the (32 - record_size_bits) offset
            uint word_mask = 1u << (bit_index % record_size_bits + 32 - record_size_bits);
            current_capture = (buf[word_index]  & word_mask) >> (bit_index % record_size_bits + 32 - record_size_bits);
            // printf("previous: %d\n", previous_buf);
            // printf("current: %d\n", current_capture);
            // printf("\n\n");
            //sleep_ms(500);
            
            if (current_capture != previous_buf) {
                //results[count] = 1; //indicate change
                printf("%d", unchange_time);
                printf(buf[word_index]  & word_mask ? "-" : "_");
                previous_buf = current_capture;
                unchange_time = 0;

            } else {
                unchange_time = unchange_time + 1;
                printf("*");
            }
            // else {
            //     //results[count] = 0; // indicate no change
            //     //strncat(results, ptr_zero, 1);
            //     nop;
            // }

        }
        printf("\n");
    }
}

void blink_capture_buf(const uint32_t *buf, uint pin_base, uint pin_count, uint32_t n_samples) {
    // Display the capture buffer in LED blink form, 1 for light, 0 for dim
    int led_status = 0;
    printf("Start Blink the captured signal:\n");
    // Each FIFO record may be only partially filled with bits, depending on
    // whether pin_count is a factor of 32.
    uint record_size_bits = bits_packed_per_word(pin_count);
    gpio_init(WS2812_POWER_PIN);
    gpio_set_dir(WS2812_POWER_PIN,true);

    neopixel_init();
    // gpio_put(WS2812_POWER_PIN,true);
    // gpio_put(WS2812_PIN,true);
    for (int pin = 0; pin < pin_count; ++pin) {
        for (int sample = 0; sample < n_samples; ++sample) {
            uint bit_index = pin + sample * pin_count;
            uint word_index = bit_index / record_size_bits;
            // Data is left-justified in each FIFO entry, hence the (32 - record_size_bits) offset
            uint word_mask = 1u << (bit_index % record_size_bits + 32 - record_size_bits);
            led_status = buf[word_index] & word_mask ? 1 : 0;
            //neopixel_init();
            if(led_status == 0) { // 
                gpio_put(WS2812_POWER_PIN, true);
                gpio_put(WS2812_PIN, true);
                neopixel_set_rgb(0x00ff0000);
                // sleep_ms(1000);
            } else { 
                gpio_put(WS2812_POWER_PIN, true);
                gpio_put(WS2812_PIN, true);
                neopixel_set_rgb(0x000000ff);  //if 0, LED dim
                // sleep_ms(1000);
            }

        }
         printf("\n");
    }
}



int main() {
    stdio_init_all();
    while(stdio_usb_connected()!=true);
    printf("Waiting\n");
    //sleep_ms(10000);
    printf("PIO logic analyser example\n");

    // We're going to capture into a u32 buffer, for best DMA efficiency. Need
    // to be careful of rounding in case the number of pins being sampled
    // isn't a power of 2.
    uint total_sample_bits = CAPTURE_N_SAMPLES * CAPTURE_PIN_COUNT;
    total_sample_bits += bits_packed_per_word(CAPTURE_PIN_COUNT) - 1;
    uint buf_size_words = total_sample_bits / bits_packed_per_word(CAPTURE_PIN_COUNT);
    uint32_t *capture_buf = malloc(buf_size_words * sizeof(uint32_t));
    hard_assert(capture_buf);

    // Grant high bus priority to the DMA, so it can shove the processors out
    // of the way. This should only be needed if you are pushing things up to
    // >16bits/clk here, i.e. if you need to saturate the bus completely.
    bus_ctrl_hw->priority = BUSCTRL_BUS_PRIORITY_DMA_W_BITS | BUSCTRL_BUS_PRIORITY_DMA_R_BITS;

    PIO pio = pio1;
    PIO pio2 = pio2;

    uint sm1 = 1;
    uint sm2 = 2;
    uint sm3 = 3;
    uint sm4 = 4;
    uint dma_chan = 1;

    logic_analyser_init(pio, sm1, CAPTURE_PIN_BASE, CAPTURE_PIN_COUNT, 64.f);


    // printf("Starting PWM example\n");
    // // PWM example: -----------------------------------------------------------
    // gpio_set_function(CAPTURE_PIN_BASE, GPIO_FUNC_PWM);
    // gpio_set_function(CAPTURE_PIN_BASE + 1, GPIO_FUNC_PWM);
    // // Topmost value of 3: count from 0 to 3 and then wrap, so period is 4 cycles
    // pwm_hw->slice[0].top = 3;
    // // Divide frequency by two to slow things down a little
    // pwm_hw->slice[0].div = 4 << PWM_CH0_DIV_INT_LSB;
    // // Set channel A to be high for 1 cycle each period (duty cycle 1/4) and
    // // channel B for 3 cycles (duty cycle 3/4)
    // pwm_hw->slice[0].cc =
    //         (1 << PWM_CH0_CC_A_LSB) |
    //         (3 << PWM_CH0_CC_B_LSB);
    // // Enable this PWM slice
    // pwm_hw->slice[0].csr = PWM_CH0_CSR_EN_BITS;
    // ------------------------------------------------------------------------

    // The logic analyser should have started capturing as soon as it saw the
    // first transition. Wait until the last sample comes in from the DMA.
   int i = 0;

    while(i < 10){
        printf("Arming trigger\n");
        logic_analyser_arm(pio, sm1, dma_chan, capture_buf, buf_size_words, TRIGGER_PIN, true);
        printf("logic analyser arm passed\n");
        sleep_ms(100);
        dma_channel_wait_for_finish_blocking(dma_chan);
        printf("dma passed\n");
        sleep_ms(100);
        print_capture_buf(capture_buf, CAPTURE_PIN_BASE, CAPTURE_PIN_COUNT, CAPTURE_N_SAMPLES, i);
        printf("print capture passed\n");
        sleep_ms(100);
        blink_capture_buf(capture_buf, CAPTURE_PIN_BASE, CAPTURE_PIN_COUNT, CAPTURE_N_SAMPLES);
        printf("blink capture passed\n");
        sleep_ms(100);

        i = i+1;
        //print_array(results);
        
    }

}


//                  cd pico/pico-examples/build/pioscope
