if (TARGET tinyusb_device)

add_executable(07_pio_sequencer)

target_sources(07_pio_sequencer PRIVATE rp2040/piosequencer.c)

pico_generate_pio_header(07_pio_sequencer ${CMAKE_CURRENT_LIST_DIR}/rp2040/sequencer.pio OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR}/rp2040/generated) # PICO SDK (PIO)

target_include_directories(07_pio_sequencer PRIVATE ${CMAKE_CURRENT_LIST_DIR}/rp2040/include ${CMAKE_CURRENT_LIST_DIR}/rp2040/generated PUBLIC ${CODE_DIR}/flashlight)
target_link_libraries(07_pio_sequencer PRIVATE pico_stdlib hardware_pio lab2_neopixel)
pico_add_extra_outputs(07_pio_sequencer)

pico_enable_stdio_usb(07_pio_sequencer 1)
pico_enable_stdio_uart(07_pio_sequencer 0)

elseif(PICO_ON_DEVICE)
    message(WARNING "not building 07_pio_sequencer
 because TinyUSB submodule is not initialized in the SDK")
endif()