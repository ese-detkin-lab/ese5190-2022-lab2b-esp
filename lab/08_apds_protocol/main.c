
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "string.h"

#define ENABLE_REG _u(0x80)
#define ALS_REG _u(0x81)
#define PDATA_REG _u(0x9C)
#define CDATAL _u(0x94)
#define GEN 0 //cCorresponidng to gesture enable register
#define AEN 1 //ALS Enable. The field activates ALS function and writing one activates the ALS. Writing a zero disables 
//The ALS detection can detect light intensity data
#define PON 1 //Power ON. This field activiates the internal oscillator to permit the timers and ADC 
//channels to operate. Writing a one activates the oscillator. Writing a zero disables the oscillator 
//and puts the device into a lower power sleep mode. During reads and writes over the I2C interface,
//this bit is temporarily overridden adn the oscillator is enabled, independent of the state of PON.
#define PIEN 1 //Proximity interrupt enable. When asserted, it permits proximity interrupts to be generated, subject to
//the persistence filter settings.
#define ALS_GAIN 0
#define ALS_TIME 219
#define PIN_SDA 22
#define PIN_SCL 23
#define CONFIGURATION (GEN << 6U) | (PIEN << 2u) | (AEN << 1u) | PON
PIO pio = pio0;
uint sm = 0;
const int addr = 0x39; //The device supports a single slave address of 0x39 Hex using 7-bit addressing protocol


const int PIO_I2C_ICOUNT_LSB = 10;
const int PIO_I2C_FINAL_LSB  = 9;
const int PIO_I2C_DATA_LSB   = 1;
const int PIO_I2C_NAK_LSB    = 0;

bool pio_i2c_check_error(PIO pio, uint sm) {
    return pio_interrupt_get(pio, sm);
}

void pio_i2c_resume_after_error(PIO pio, uint sm) {
    pio_sm_drain_tx_fifo(pio, sm);
    pio_sm_exec(pio, sm, (pio->sm[sm].execctrl & PIO_SM0_EXECCTRL_WRAP_BOTTOM_BITS) >> PIO_SM0_EXECCTRL_WRAP_BOTTOM_LSB);
    pio_interrupt_clear(pio, sm);
}

void pio_i2c_rx_enable(PIO pio, uint sm, bool en) {
    if (en)
        hw_set_bits(&pio->sm[sm].shiftctrl, PIO_SM0_SHIFTCTRL_AUTOPUSH_BITS);
    else
        hw_clear_bits(&pio->sm[sm].shiftctrl, PIO_SM0_SHIFTCTRL_AUTOPUSH_BITS);
}

static inline void pio_i2c_put16(PIO pio, uint sm, uint16_t data) {
    while (pio_sm_is_tx_fifo_full(pio, sm))
        ;
    // some versions of GCC dislike this
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
    *(io_rw_16 *)&pio->txf[sm] = data;
#pragma GCC diagnostic pop
}


// If I2C is ok, block and push data. Otherwise fall straight through.
void pio_i2c_put_or_err(PIO pio, uint sm, uint16_t data) {
    while (pio_sm_is_tx_fifo_full(pio, sm))
        if (pio_i2c_check_error(pio, sm))
            return;
    if (pio_i2c_check_error(pio, sm))
        return;
    // some versions of GCC dislike this
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
    *(io_rw_16 *)&pio->txf[sm] = data;
#pragma GCC diagnostic pop
}

uint8_t pio_i2c_get(PIO pio, uint sm) {
    return (uint8_t)pio_sm_get(pio, sm);
}

void pio_i2c_start(PIO pio, uint sm) {
    pio_i2c_put_or_err(pio, sm, 1u << PIO_I2C_ICOUNT_LSB); // Escape code for 2 instruction sequence
    pio_i2c_put_or_err(pio, sm, set_scl_sda_program_instructions[I2C_SC1_SD0]);    // We are already in idle state, just pull SDA low
    pio_i2c_put_or_err(pio, sm, set_scl_sda_program_instructions[I2C_SC0_SD0]);    // Also pull clock low so we can present data
}

void pio_i2c_stop(PIO pio, uint sm) {
    pio_i2c_put_or_err(pio, sm, 2u << PIO_I2C_ICOUNT_LSB);
    pio_i2c_put_or_err(pio, sm, set_scl_sda_program_instructions[I2C_SC0_SD0]);    // SDA is unknown; pull it down
    pio_i2c_put_or_err(pio, sm, set_scl_sda_program_instructions[I2C_SC1_SD0]);    // Release clock
    pio_i2c_put_or_err(pio, sm, set_scl_sda_program_instructions[I2C_SC1_SD1]);    // Release SDA to return to idle state
};

void pio_i2c_repstart(PIO pio, uint sm) {
    pio_i2c_put_or_err(pio, sm, 3u << PIO_I2C_ICOUNT_LSB);
    pio_i2c_put_or_err(pio, sm, set_scl_sda_program_instructions[I2C_SC0_SD1]);
    pio_i2c_put_or_err(pio, sm, set_scl_sda_program_instructions[I2C_SC1_SD1]);
    pio_i2c_put_or_err(pio, sm, set_scl_sda_program_instructions[I2C_SC1_SD0]);
    pio_i2c_put_or_err(pio, sm, set_scl_sda_program_instructions[I2C_SC0_SD0]);
}

static void pio_i2c_wait_idle(PIO pio, uint sm) {
    // Finished when TX runs dry or SM hits an IRQ
    pio->fdebug = 1u << (PIO_FDEBUG_TXSTALL_LSB + sm);
    while (!(pio->fdebug & 1u << (PIO_FDEBUG_TXSTALL_LSB + sm) || pio_i2c_check_error(pio, sm)))
        tight_loop_contents();
}

int pio_i2c_write_blocking(PIO pio, uint sm, uint8_t addr, uint8_t *txbuf, uint len,bool sign) {
    int err = 0;
    pio_i2c_start(pio, sm);
    pio_i2c_rx_enable(pio, sm, false);
    pio_i2c_put16(pio, sm, (addr << 2) | 1u);
    while (len && !pio_i2c_check_error(pio, sm)) {
        if (!pio_sm_is_tx_fifo_full(pio, sm)) {
            --len;
            pio_i2c_put_or_err(pio, sm, (*txbuf++ << PIO_I2C_DATA_LSB) | ((len == 0) << PIO_I2C_FINAL_LSB) | 1u);
        }
    }
    if(!sign)
    {
        pio_i2c_stop(pio, sm);
    }
    pio_i2c_wait_idle(pio, sm);
    if (pio_i2c_check_error(pio, sm)) {
        err = -1;
        pio_i2c_resume_after_error(pio, sm);
        pio_i2c_stop(pio, sm);
    }
    return err;
}

int pio_i2c_read_blocking(PIO pio, uint sm, uint8_t addr, uint8_t *rxbuf, uint len,bool sign) {
    int err = 0;
    //!!!Modify the start into repstart
    pio_i2c_repstart(pio, sm);
    pio_i2c_rx_enable(pio, sm, true);
    while (!pio_sm_is_rx_fifo_empty(pio, sm))
        (void)pio_i2c_get(pio, sm);
    pio_i2c_put16(pio, sm, (addr << 2) | 3u);
    uint32_t tx_remain = len; // Need to stuff 0xff bytes in to get clocks

    bool first = true;

    while ((tx_remain || len) && !pio_i2c_check_error(pio, sm)) {
        if (tx_remain && !pio_sm_is_tx_fifo_full(pio, sm)) {
            --tx_remain;
            pio_i2c_put16(pio, sm, (0xffu << 1) | (tx_remain ? 0 : (1u << PIO_I2C_FINAL_LSB) | (1u << PIO_I2C_NAK_LSB)));
        }
        if (!pio_sm_is_rx_fifo_empty(pio, sm)) {
            if (first) {
                // Ignore returned address byte
                (void)pio_i2c_get(pio, sm);
                first = false;
            }
            else {
                --len;
                *rxbuf++ = pio_i2c_get(pio, sm);
            }
        }
    }
    pio_i2c_stop(pio, sm);
    pio_i2c_wait_idle(pio, sm);
    if (pio_i2c_check_error(pio, sm)) {
        err = -1;
        pio_i2c_resume_after_error(pio, sm);
        pio_i2c_stop(pio, sm);
    }
    return err;
}

void APDS9960_init() {
    uint8_t buf[2];
    //Set the corresponidng register number and initialzie APDS9960
    buf[0] = ENABLE_REG;
    buf[1] = CONFIGURATION;
    pio_i2c_write_blocking(pio, sm, addr, buf, 2, false);
    buf[0] = ALS_REG;
    buf[1] = ALS_TIME;
    pio_i2c_write_blocking(pio, sm, addr, buf, 2, false);
}
void readProximity(int32_t* proximity)
{
    uint8_t buf[1];
    uint8_t proximityReg = PDATA_REG;
    pio_i2c_write_blocking(pio, sm, addr, &proximityReg, 1, true); //try to keep master control of the bus
    pio_i2c_read_blocking(pio, sm, addr, buf, 1, false);
    *proximity = buf[0];

}
int main() {
    stdio_init_all();
    uint offset = pio_add_program(pio, &i2c_program);
    i2c_program_init(pio, sm, offset, PIN_SDA, PIN_SCL);
    sleep_ms(5000);
    APDS9960_init();
    printf("\nPIO I2C Bus Scan\n");
    printf("\nThis is part 8 with reading raw data from APDS9960.\n");
    while (1)
    {
        int32_t proximity;
        int32_t r, g, b, c;
        readProximity(&proximity);
        printf("proximity: %d\n", proximity);
    }
}
