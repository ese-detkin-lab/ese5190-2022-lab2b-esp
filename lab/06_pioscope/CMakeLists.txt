if (TARGET tinyusb_device)

add_executable(06_pioscope)

target_sources(06_pioscope PRIVATE logic_analyser.c)

target_include_directories(06_pioscope PUBLIC ${CODE_DIR}/flashlight)
target_link_libraries(06_pioscope PRIVATE pico_stdlib hardware_pio hardware_dma lab2_neopixel)
pico_add_extra_outputs(06_pioscope)

pico_enable_stdio_usb(06_pioscope 1)
pico_enable_stdio_uart(06_pioscope 0)

elseif(PICO_ON_DEVICE)
    message(WARNING "not building 06_pioscope
 because TinyUSB submodule is not initialized in the SDK")
endif()