    cmake_minimum_required(VERSION 3.13)

    # Enable exporting compile commands for IntelliSense
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

    # find source files
    file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS src/**.c src/**.h) #PLZ use GLOB_RECURSE not GLOB or the linker will be annoyed
    message(STATUS "Included source files: \n${SRC_FILES}\n\n")

    # initialize the SDK based on PICO_SDK_PATH
    # note: this must happen before project()
    include(pico_sdk_import.cmake)

### start project
project(ETA-klocka)

    # initialize the Raspberry Pi Pico SDK
    pico_sdk_init()

    # rest of your project
    add_executable(main)

        # platform specific
        pico_enable_stdio_usb(main ENABLED)   # links standard output stream to usb serial (useful for debugging)
        pico_add_extra_outputs(main)          # output map/bin/hex/uf2 file in addition to ELF.

        # include
        target_sources(main PRIVATE src/main.c src/driver/lcd.c)                     # C source files
        pico_generate_pio_header(main ${CMAKE_CURRENT_LIST_DIR}/src/driver/lcd.pio)  # PIO assembly source files
        target_link_libraries(main pico_stdlib hardware_pio hardware_dma)            # Add pico_stdlib library which aggregates commonly used features


