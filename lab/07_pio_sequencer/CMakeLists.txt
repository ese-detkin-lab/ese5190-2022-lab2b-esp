if (TARGET tinyusb_device)

add_custom_target(pre_build_version_update)
add_custom_command(
    TARGET pre_build_version_update
    PRE_BUILD
    WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}"
    COMMAND ${CMAKE_COMMAND}
        -P "${ESE5190_LAB2B_DIR}/auto_version.cmake"
)


add_executable(07_pio_sequencer)
add_dependencies(07_pio_sequencer pre_build_version_update)
target_sources(07_pio_sequencer PRIVATE rp2040/piosequencer.c)

pico_generate_pio_header(07_pio_sequencer ${CMAKE_CURRENT_LIST_DIR}/rp2040/sequencer.pio OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR}/rp2040/generated) # PICO SDK (PIO)

target_include_directories(07_pio_sequencer PRIVATE ${CMAKE_CURRENT_LIST_DIR}/rp2040/include ${CMAKE_CURRENT_LIST_DIR}/rp2040/generated)
target_link_libraries(07_pio_sequencer PRIVATE pico_stdlib hardware_pio hardware_dma)
pico_add_extra_outputs(07_pio_sequencer)

pico_enable_stdio_usb(07_pio_sequencer 1)
pico_enable_stdio_uart(07_pio_sequencer 0)



elseif(PICO_ON_DEVICE)
    message(WARNING "not building 07_pio_sequencer
 because TinyUSB submodule is not initialized in the SDK")
endif()