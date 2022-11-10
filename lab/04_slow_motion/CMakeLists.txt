if (TARGET tinyusb_device)

add_executable(04_slow_motion)

target_sources(04_slow_motion PRIVATE rp2040/deltasequencer.c)

target_include_directories(04_slow_motion INTERFACE ${CMAKE_CURRENT_LIST_DIR}/rp2040/include PUBLIC ${CODE_DIR}/flashlight)
target_link_libraries(04_slow_motion PRIVATE pico_stdlib lab2_neopixel)
pico_add_extra_outputs(04_slow_motion)

pico_enable_stdio_usb(04_slow_motion 1)
pico_enable_stdio_uart(04_slow_motion 0)

elseif(PICO_ON_DEVICE)
    message(WARNING "not building 04_slow_motion
 because TinyUSB submodule is not initialized in the SDK")
endif()