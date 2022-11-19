#ifndef APDS_H
#define APDS_H

#define ENABLE_REG      _u(0x80)
#define PDATA_REG       _u(0x9C)
#define CDATA_REG_L     _u(0x94)
#define CDATA_REG_H     _u(0x95) 
#define RDATA_REG_L     _u(0x96)
#define RDATA_REG_H     _u(0x97)
#define GDATA_REG_L     _u(0x98)
#define GDATA_REG_H     _u(0x99)
#define BDATA_REG_L     _u(0x9A)
#define BDATA_REG_H     _u(0x9B)

#define CONFIG_REG_1    _u(0x8F)
#define CONFIG_REG_2    _u(0x90)
#define CONFIG_REG_3    _u(0x9F)
#define ALS_TIME_REG    _u(0x81)

#define POWER_ENABLE 1
#define PROXIMITY_ENABLE 1
#define ALS_ENABLE 1
#define GESTURE_ENABLE 0

#define ALS_GAIN 0
#define ALS_TIME 219

#define INIT_CONFIG (GESTURE_ENABLE << 6u) | (PROXIMITY_ENABLE << 2u) | (ALS_ENABLE << 1u) | POWER_ENABLE

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "string.h"
#include "pio_i2c.h"

void init_sensor();
void proximity_sensor(int32_t *proximity, PIO pio, uint sm, uint32_t addr, bool continue_do);
void read_rgbc(int32_t* r, int32_t* g, int32_t* b, int32_t* c);


#endif
