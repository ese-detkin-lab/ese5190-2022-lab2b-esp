include(pico_sdk_import.cmake)
project(logic_analyzer)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)


pico_sdk_init()
add_executable(logic_analyzer
        logic_analyzer.c
        )

pico_enable_stdio_usb(logic_analyzer 1)
pico_enable_stdio_uart(logic_analyzer 0)

target_link_libraries(logic_analyser pico_stdlib hardware_pio hardware_dma hardware_pwm) 
pico_add_extra_outputs(pio_logic_analyser)
