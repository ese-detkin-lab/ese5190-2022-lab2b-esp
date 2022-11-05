if (TARGET tinyusb_device)

add_executable(02_repl)

target_sources(02_repl PRIVATE repl.c)

target_include_directories(02_repl INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include PUBLIC ${CODE_DIR}/flashlight)
target_link_libraries(02_repl PRIVATE pico_stdlib lab2_neopixel)
pico_add_extra_outputs(02_repl)

pico_enable_stdio_usb(02_repl 1)
pico_enable_stdio_uart(02_repl 0)

elseif(PICO_ON_DEVICE)
    message(WARNING "not building 02_repl
 because TinyUSB submodule is not initialized in the SDK")
endif()
