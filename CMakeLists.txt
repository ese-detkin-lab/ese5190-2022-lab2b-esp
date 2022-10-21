add_executable(code blink.c)

pico_enable_stdio_usb(code 1)
pico_enable_stdio_uart(code 0)


target_link_libraries(code pico_stdlib)

pico_add_extra_outputs(code)