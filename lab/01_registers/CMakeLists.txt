if (TARGET tinyusb_device)

add_executable(01_registers)

target_sources(01_registers PRIVATE registers.c)

target_include_directories(01_registers INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include PUBLIC ${CODE_DIR}/flashlight)
target_link_libraries(01_registers PRIVATE pico_stdlib lab2_neopixel)
pico_add_extra_outputs(01_registers)

pico_enable_stdio_usb(01_registers 1)
pico_enable_stdio_uart(01_registers 0)

elseif(PICO_ON_DEVICE)
    message(WARNING "not building 01_registers
 because TinyUSB submodule is not initialized in the SDK")
endif()