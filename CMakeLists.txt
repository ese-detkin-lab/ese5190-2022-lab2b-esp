add_executablemax(7219_8x8dot_spi
        max7219_8x8dot_spi.c
        )

# pull in common dependencies and additional spi hardware support
target_link_libraries(max7219_8x8dot_spi pico_stdlib hardware_spi)

# create map/bin/hex file etc.
pico_add_extra_outputs(max7219_8x8dot_spi)

pico_enable_stdio_usb(max7219_8x8dot_spi 1)
pico_enable_stdio_uart(max7219_8x8dot_spi 0)

# add url via pico_set_program_url
example_auto_set_url(max7219_8x8dot_spi)
