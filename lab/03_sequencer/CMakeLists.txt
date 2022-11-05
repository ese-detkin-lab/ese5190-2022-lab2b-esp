if (TARGET tinyusb_device)

add_executable(03_sequencer)

target_sources(03_sequencer PRIVATE rp2040/sequencer.c)

target_include_directories(03_sequencer INTERFACE ${CMAKE_CURRENT_LIST_DIR}/rp2040/include PUBLIC ${CODE_DIR}/flashlight)
target_link_libraries(03_sequencer PRIVATE pico_stdlib lab2_neopixel)
pico_add_extra_outputs(03_sequencer)

pico_enable_stdio_usb(03_sequencer 1)
pico_enable_stdio_uart(03_sequencer 0)

elseif(PICO_ON_DEVICE)
    message(WARNING "not building 03_sequencer
 because TinyUSB submodule is not initialized in the SDK")
endif()