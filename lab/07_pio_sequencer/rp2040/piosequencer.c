#include "pico/stdlib.h"
#include <stdio.h>
#include <string.h>
#include "hardware/gpio.h"

#include "serial_interface.h"
#include "sequencer.pio.h"
#define INCREMENTED_VALUE 0 //#include "increment.h"

#define QTPY_BOOT_PIN 21
#define QTPY_BOOT_PIN_BITMASK 0x200000
#define QTPY_GPIO_A0_BITMASK 0x20000000
#define GPIO_IN_OFFSET 0x4

#define TX_ADDR_PIO_SM 0
#define TX_VALS_PIO_SM 1
#define TX_TS_PIO_SM 2
#define TX_PINS_PIO_SM 3

typedef volatile uint32_t* regaddr;

uint addr_dma_chan;
uint val_dma_chan;
uint ts_dma_chan;
uint val_dma_out_chan;
uint addr_dma_out_chan;

// defines which register to use as input/output for the sequencer, and if that input is atomic
typedef struct {
    volatile uint32_t* address;
    uint32_t mask;
    bool is_binary;
} inoutreg;

typedef struct {
    uint32_t delta; //time from start of recording/playback
    inoutreg reg;
    uint32_t value;
} datapoint;

#define STORAGE_BUFFER_LENGTH 1000
typedef struct {
    //datapoint data[STORAGE_BUFFER_LENGTH];
    uint32_t addresses[STORAGE_BUFFER_LENGTH];
    uint32_t data[STORAGE_BUFFER_LENGTH];
    uint32_t timestamps[STORAGE_BUFFER_LENGTH];
    //uint32_t masks[STORAGE_BUFFER_LENGTH];
    int index;
    int recordlen; //set to index upon write, not reset for read/replay.
    absolute_time_t starttime;
} seqstorage;

const seqstorage STORAGE_INIT = {.index = 0, .recordlen = 0};
seqstorage store;

//only track off/on for each color and dim/bright for all colors, let's not go crazy...
typedef struct {
    bool red;
    bool green;
    bool blue;
    bool is_bright;
} ledstate;

typedef struct {
    bool boot_button;
    bool serial;
    bool register_read;
} inputsettings;

typedef struct {
    bool led_color;
    bool led_brightness;
    bool serial_out;
    bool register_write;
    float scale;
} outputsettings;

inoutreg bootpin = {(volatile uint32_t*)(SIO_BASE | SIO_GPIO_IN_OFFSET), QTPY_BOOT_PIN_BITMASK, true};
inputsettings INPUT_SETTINGS = {false, false, false};
outputsettings OUTPUT_SETTINGS = {false, false, false, false, 1.};

uint32_t test_var = 0;

// store value in buffer, return true if buffer did not become full (can store more)
bool write_event_to_storage(seqstorage* store, uint32_t data, inoutreg* reg, uint32_t delta) {
    //if the data in this event is the same as the data in the last recorded point, do not store it
    if ((store->index != 0) && (store->data[store->index - 1] == data)) {return (store->index < STORAGE_BUFFER_LENGTH);}
    
    store->data[store->index] = data;
    store->addresses[store->index] = (uint32_t)reg->address;
    store->timestamps[store->index] = delta; //absolute_time_diff_us(store->starttime, get_absolute_time());
    store->recordlen = store->index;
    store->index++;
    

    return (store->index < STORAGE_BUFFER_LENGTH);
}

// return next value in buffer, return false if all values have been read.
bool read_event_from_storage(seqstorage* store, uint32_t* data, inoutreg* reg, bool* newdata, float scale) {
    if (store->index >= store->recordlen) {return false;}
    *newdata = false;
    //if the time between now and the start of playback is less than the stored delta, do not output
    if (absolute_time_diff_us(store->starttime, get_absolute_time()) < (store->timestamps[store->index] / scale)) {return (store->index <= store->recordlen);}
    //*reg = store->data[store->index].reg;
    *data = store->data[store->index];
    store->index++;
    *newdata = true;
    return (store->index <= store->recordlen);
}

void import_sequence(seqstorage* store) {
    bool storage_available = true;
    store->index = 0;
    uint32_t val = 0;
    uint32_t delta = 0;
    inoutreg reg;
    
    while (storage_available) {
        //putchar(store->index + 48);
        if (!get_hex(&val)) {putchar('\x3B'); break;}
        delta = val;
        if (!get_hex(&val)) {putchar('\x3B'); break;}
        reg.address = (volatile uint32_t*)val;
        //if (!get_hex(&val)) {putchar('\x3B'); return;}
        //reg.mask = val;
        if (!get_hex(&val)) {putchar('\x3B'); break;}

        storage_available = write_event_to_storage(store, val, &reg, delta);
        //store->timestamps[store->index - 1] = delta; //go back in time and overwrite the delta
    }
    inoutreg temp = {.address = &test_var, .mask = 0, .is_binary = false};
    write_event_to_storage(store, 0xDEADBEEF, &temp, 0x5);
    temp.address = 0x0;
    //write_event_to_storage(store, 0x0, &temp, 0xff);
}

void export_sequence(seqstorage* store) {
    store->index = 0;
    printf("version:%d\n", INCREMENTED_VALUE);
    while(store->index <= store->recordlen) {
        printf("%08X,%08X,%08X\n", store->timestamps[store->index], store->addresses[store->index], store->data[store->index]);
        store->index++;
    }
    printf("\x3B\n"); //write semicolon to signal EOF
}

/*
//call this every loop while recording
// return false if storage full
bool record_sequence(inoutreg* inputreg, seqstorage* store, uint32_t serial) {
    bool continuerecording = false;
    uint32_t val;
    if (absolute_time_diff_us(store->starttime, get_absolute_time()) > 10000000) {return false;}
    if (INPUT_SETTINGS.boot_button) {
        //val = (read_register_value(&bootpin));
        continuerecording = write_event_to_storage(store, val, inputreg);
    }
    if (INPUT_SETTINGS.serial) {
        //roominbuffer = write_event_to_storage(store, serial);
    }
    if (INPUT_SETTINGS.register_read) {
        //val = read_register_value(inputreg);
        continuerecording = write_event_to_storage(store, val, inputreg);
    }
    return continuerecording;
}
*/

void replay_sequence() {

    //dma_sniffer_enable(addr_dma_chan, 0, true);
    pio_sm_set_enabled(pio0, 0, false);
    pio_sm_set_enabled(pio0, 1, false);
    pio_sm_set_enabled(pio0, 2, false);
    pio_sm_set_enabled(pio0, 3, false);
    pio_restart_sm_mask(pio0, 0xf);
    pio_sm_clear_fifos(pio0, 0);
    pio_sm_clear_fifos(pio0, 1);
    pio_sm_clear_fifos(pio0, 2);
    pio_sm_clear_fifos(pio0, 3);

    pio_enable_sm_mask_in_sync(pio0, 0xf);

    //TODO: need to reset DMAs that may be running

}

void stop_replay() {

}

void pio_tx_rx_init() {
    PIO pio = pio0;
    uint dma_addr = dma_claim_unused_channel(true);
    uint dma_val = dma_claim_unused_channel(true);
    uint dma_ts = dma_claim_unused_channel(true);
    uint dma_addr_out = dma_claim_unused_channel(true);
    uint dma_val_out = dma_claim_unused_channel(true);
    addr_dma_chan = dma_addr;
    val_dma_chan = dma_val;
    ts_dma_chan = dma_ts;
    val_dma_out_chan = dma_val_out;
    addr_dma_out_chan = dma_addr_out;

    uint offset = pio_add_program(pio, &sequencer_tx_addrs_vals_program);
    printf("%d ", offset);
    sequencer_tx_addrs_vals_init(pio, TX_ADDR_PIO_SM, TX_VALS_PIO_SM, offset, dma_addr_out, dma_val_out);

    offset = pio_add_program(pio, &sequencer_tx_timestamps_program);
    printf("%d ", offset);
    sequencer_tx_timestamps_init(pio, TX_TS_PIO_SM, offset);

    offset = pio_add_program(pio, &sequencer_tx_pins_program);
    printf("%d \n", offset);
    sequencer_tx_pins_init(pio, TX_PINS_PIO_SM, offset);

    //address struct -> SM0 TX FIFO
    dma_channel_config c = dma_channel_get_default_config(dma_addr);
    channel_config_set_read_increment(&c, true);
    channel_config_set_write_increment(&c, false);
    channel_config_set_dreq(&c, pio_get_dreq(pio, TX_ADDR_PIO_SM, false));

    dma_channel_configure(dma_addr, &c,
        &pio->txf[TX_ADDR_PIO_SM],      // Destination pointer
        store.addresses,    // Source pointer
        1,             // Number of transfers. Set to "a lot"
        true                // Start immediately
    );

    //values struct -> SM1 TX FIFO
    c = dma_channel_get_default_config(dma_val);
    channel_config_set_read_increment(&c, true);
    channel_config_set_write_increment(&c, false);
    channel_config_set_dreq(&c, pio_get_dreq(pio, TX_VALS_PIO_SM, false));

    dma_channel_configure(dma_val, &c,
        &pio->txf[TX_VALS_PIO_SM],    // Destination pointer
        store.data,         // Source pointer
        1,             // Number of transfers. Set to "a lot"
        true                // Start immediately
    );

    //timestamps struct -> SM2 TX FIFO
    c = dma_channel_get_default_config(dma_ts);
    channel_config_set_read_increment(&c, true);
    channel_config_set_write_increment(&c, false);
    channel_config_set_dreq(&c, pio_get_dreq(pio, TX_TS_PIO_SM, false));

    dma_channel_configure(dma_ts, &c,
        &pio->txf[TX_TS_PIO_SM],    // Destination pointer
        store.timestamps,   // Source pointer
        1,             // Number of transfers. Set to "a lot"
        true                // Start immediately
    );
}

void report_error(char* msg) {
    putchar('E');
    printf(msg);
    printf("\n");
}

void print_debug_regs(bool print_header) {

    if(print_header){printf("pio0ctrl,sm0execc,sm1execc,sm2execc,sm3execc,fdebug  ,fstat   ,flevel  ,test_var,dbpadout,dbgpadoe,sm3pctrl,dmavora ,dmavowa ,dmaaora ,dmaaowa ,dmavira ,dmaviwa ,dmaaira ,dmaaiwa ,dmatsra ,dmatswa\n");}
    printf("%08X,%08X,%08X,%08X,%08X,%08X,%08X,%08X,%08X,%08X,%08X,%08X,%08X,%08X,%08X,%08X,%08X,%08X,%08X,%08X,%08X,%08X\n", 
        pio0->ctrl,
        (pio0->sm[0].execctrl),
        (pio0->sm[1].execctrl),
        (pio0->sm[2].execctrl),
        (pio0->sm[3].execctrl),
        (pio0->fdebug),
        (pio0->fstat),
        pio0->flevel,
        test_var,
        pio0->dbg_padout,
        pio0->dbg_padoe,
        pio0->sm[3].pinctrl,
        dma_hw->ch[val_dma_out_chan].read_addr,
        dma_hw->ch[val_dma_out_chan].write_addr,
        dma_hw->ch[addr_dma_out_chan].read_addr,
        dma_hw->ch[addr_dma_out_chan].write_addr,
        dma_hw->ch[val_dma_chan].read_addr,
        dma_hw->ch[val_dma_chan].write_addr,
        dma_hw->ch[addr_dma_chan].read_addr,
        dma_hw->ch[addr_dma_chan].write_addr,
        dma_hw->ch[ts_dma_chan].read_addr,
        dma_hw->ch[ts_dma_chan].write_addr
         
        );

}

int main() {
    bool recordactive = false;
    bool playbackactive = false;
    bool loopplayback = false;
    
    stdio_init_all();
    gpio_init(QTPY_BOOT_PIN);
    gpio_set_dir(QTPY_BOOT_PIN, GPIO_IN);


    //proof of life
    gpio_init(22);
    gpio_set_dir(22, GPIO_OUT);
    gpio_set_dir(23, GPIO_OUT);
    for(int i = 0; i < 10; i++){
        gpio_set_mask(1u << 22);
        sleep_ms(20);
        gpio_clr_mask(1u << 22);
        sleep_ms(20);
    }

    gpio_set_function(22, GPIO_FUNC_PIO0);
    gpio_set_function(23, GPIO_FUNC_PIO0);

    while(!stdio_usb_connected());
    report_error("test error everything is fine");

     //init storage struct
    store = STORAGE_INIT;

    ledstate LED_STATE = {false, false, false, false};

    pio_tx_rx_init();

    while (true) {
        int c = getchar_timeout_us(0);
        if(c == PICO_ERROR_TIMEOUT) {

        }
        else if(c != '\n' && c != '\r' && c != PICO_ERROR_TIMEOUT) {
            /* 
             *  Data (registers and data to read/write) will be sent in binary form
             *  All dword data will be preceeded by a character that denotes what it is
             *  e.g. 'r'0xffffffff is an input register change and the new register address
             *  Some characters are atomic operations, e.g. activate/deactivate an input type 
             */
            switch (c) {
                case 'B': 
                    INPUT_SETTINGS.boot_button = true;
                    //inputregister = bootpin;
                    putchar('B');
                    break;
                case 'b':
                    INPUT_SETTINGS.boot_button = false;
                    putchar('b');
                    break;
                case 'C':
                    OUTPUT_SETTINGS.led_color = true;
                    OUTPUT_SETTINGS.led_brightness = false;
                    LED_STATE = (ledstate){true, false, false, false};
                    putchar('l');
                    putchar('C');
                    //set_led_state(&LED_STATE);
                    break;
                case 'c':
                    OUTPUT_SETTINGS.led_color = false;
                    putchar('c');
                    break;
                case 'L':
                    OUTPUT_SETTINGS.led_brightness = true;
                    OUTPUT_SETTINGS.led_color = false;
                    putchar('c');
                    putchar('L');
                    LED_STATE = (ledstate){true, false, true, false};//the yellow was giving me a headache
                    //set_led_state(&LED_STATE);
                    break;
                case 'l':
                    OUTPUT_SETTINGS.led_brightness = false;
                    putchar('l');
                    break;
                case 'W':
                    if (!(playbackactive || recordactive)) {
                        OUTPUT_SETTINGS.register_write= true;
                        putchar('W');
                    }
                    break;
                case 'w':
                    OUTPUT_SETTINGS.register_write = false;
                    putchar('w');
                    break;    
                case 'X':
                    recordactive = true; //start a recording
                    store.index = 0;
                    putchar('X');
                    store.starttime = get_absolute_time();
                    break;
                case 'x':
                    recordactive = false;//terminate a recording early
                    putchar('x');
                    break;
                case 'P':
                    playbackactive = true; //start playback
                    store.index = 0;
                    putchar('P');
                    printf("Pre-run values:\ntest_addr:%08X\tval:%08X\n", &test_var, test_var);
                    print_debug_regs(true);
                    replay_sequence();
                    /*int opt = getchar();
                    switch (opt) {
                        case '1':
                            OUTPUT_SETTINGS.scale = 2;
                            break;
                        case '2':
                            OUTPUT_SETTINGS.scale = 0.5;
                            break;
                        case '3':
                            OUTPUT_SETTINGS.scale = 0.25;
                            break;
                        default:
                            OUTPUT_SETTINGS.scale = 1;
                    }
                    putchar('P');*/
                    //store.starttime = get_absolute_time();
                    printf("Post-run register states:\n");

                    break;
                case 'p':
                    playbackactive = false;//terminate playback
                    stop_replay();
                    putchar('p');
                    break;
                case 'O':
                    loopplayback = true;
                    putchar('O');
                    break;
                case 'o':
                    loopplayback = false;
                    putchar('o');
                    break;
                case 'U':
                    putchar('U');
                    export_sequence(&store);
                    putchar('u');
                    break;
                case 'D':
                    putchar('D');
                    import_sequence(&store);
                    putchar('d');
                    break;
                default:
                    
                    break;
            }
        }
        if(recordactive) {
            //recordactive = record_sequence(&inputregister, &store, serinput);
            if (!recordactive) {putchar('x');}
        }
        if (playbackactive) {
            //playbackactive = dma_channel_is_busy() //TODO: indicate when playback is over (use status of timestamp DMA)
            // how to know when it's finished if "number of transfers" is arbitrarily high?
            print_debug_regs(false);
            if (!playbackactive && loopplayback) {
                playbackactive = true;
                store.index = 0;
                store.starttime = get_absolute_time();
            }
            if (!playbackactive) {putchar('p');}
        }
        sleep_ms(10);
    }
    return 0;
    
}                  


