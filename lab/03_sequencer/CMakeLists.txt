cmake_minimum_required(VERSION 3.13) # CMAKE (heading)
include(pico_sdk_import.cmake) # CMAKE -> PICO SDK (include)

project(sequencer C CXX ASM) # CMAKE ("project")
pico_sdk_init() # PICO SDK (initialization)

add_executable(sequencer) # CMAKE (target)

pico_generate_pio_header(sequencer ${CMAKE_CURRENT_LIST_DIR}/ws2812.pio OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR}/generated) # PICO SDK (PIO)

target_sources(sequencer PRIVATE sequencer.c register.h neopixel.c)

target_link_libraries(sequencer PRIVATE pico_stdlib hardware_pio) # CMAKE -> C linker)

pico_enable_stdio_usb(sequencer 1) # PICO SDK (USB)
pico_enable_stdio_uart(sequencer 0) # PICO SDK (USB)

pico_add_extra_outputs(sequencer) # PICO SDK (UF2)
