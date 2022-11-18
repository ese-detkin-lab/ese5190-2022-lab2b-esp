#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/dma.h"
#include "hardware/structs/bus_ctrl.h"

// Some logic to analyse:
#include "hardware/structs/pwm.h"

#define QTPY_BOOT_PIN 21

#define PIO			pio0
#define SM			0
#define DMA_CHAN 0

const uint CAPTURE_PIN_BASE = QTPY_BOOT_PIN;
const uint CAPTURE_PIN_COUNT = 1;
const uint CAPTURE_N_SAMPLES = 1;

// void record(Register_status **ram, uint32_t data){
//     Register_status ram_new;
//     ram_new -> reg_address = ram.reg_address + 4;
//     register_write(address,data);
//     ram_new->reg_value = 0;
//     *ram = ram_new;
// }

static inline uint bits_packed_per_word(uint pin_count) {
    // If the number of pins to be sampled divides the shift register size, we
    // can use the full SR and FIFO width, and push when the input shift count
    // exactly reaches 32. If not, we have to push earlier, so we use the FIFO
    // a little less efficiently.
    const uint SHIFT_REG_WIDTH = 32;
    return SHIFT_REG_WIDTH - (SHIFT_REG_WIDTH % pin_count);
}

void logic_analyser_init(uint pin_base, uint pin_count, float div) {
    // Load a program to capture n pins. This is just a single `in pins, n`
    // instruction with a wrap.
    uint16_t capture_prog_instr = pio_encode_in(pio_pins, pin_count);
    struct pio_program capture_prog = {
            .instructions = &capture_prog_instr,
            .length = 1,
            .origin = -1
    };
    uint offset = pio_add_program(PIO, &capture_prog);

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
    pio_sm_init(PIO, SM, offset, &c);
}

void logic_analyser_arm(uint32_t *capture_buf, size_t capture_size_words,
                        uint trigger_pin, bool trigger_level) {
    pio_sm_set_enabled(PIO, SM, false);
    // Need to clear _input shift counter_, as well as FIFO, because there may be
    // partial ISR contents left over from a previous run. sm_restart does this.
    pio_sm_clear_fifos(PIO, SM);
    pio_sm_restart(PIO, SM);

    dma_channel_config c = dma_channel_get_default_config(DMA_CHAN);
    channel_config_set_read_increment(&c, false);
    channel_config_set_write_increment(&c, true);
    channel_config_set_dreq(&c, pio_get_dreq(PIO, SM, false));

    dma_channel_configure(DMA_CHAN, &c,
        capture_buf,        // Destination pointer
        &PIO->rxf[SM],      // Source pointer
        capture_size_words, // Number of transfers
        true                // Start immediately
    );

    // pio_sm_exec(PIO, SM, pio_encode_wait_gpio(trigger_level, trigger_pin));
    pio_sm_set_enabled(PIO, SM, true);
}

void print_capture_buf(const uint32_t *buf, uint pin_base, uint pin_count, uint32_t n_samples) {
    // Display the capture buffer in text form, like this:
    // 00: __--__--__--__--__--__--
    // 01: ____----____----____----
    printf("BOOT PIN Capture:\n");
    // Each FIFO record may be only partially filled with bits, depending on
    // whether pin_count is a factor of 32.
    uint record_size_bits = bits_packed_per_word(pin_count);

    for (int sample = 0; sample < n_samples; ++sample) {
        uint bit_index = pin_base + sample;
        uint word_index = bit_index / record_size_bits;
        // Data is left-justified in each FIFO entry, hence the (32 - record_size_bits) offset
        uint word_mask = 1u << (bit_index % record_size_bits + 32 - record_size_bits);
        printf(buf[word_index] & word_mask ? "0" : "1");
    }
    printf("\n");
}

int main() {
    stdio_init_all();
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

    logic_analyser_init(CAPTURE_PIN_BASE, CAPTURE_PIN_COUNT, 1.f);

    while(true){
        uint32_t ans = getchar_timeout_us(0);
        logic_analyser_arm(capture_buf, buf_size_words, CAPTURE_PIN_BASE, false);
        print_capture_buf(capture_buf, CAPTURE_PIN_BASE, CAPTURE_PIN_COUNT, CAPTURE_N_SAMPLES);
        if(ans == 'N'){
            break;
        }
        sleep_ms(1000);
    }

}
