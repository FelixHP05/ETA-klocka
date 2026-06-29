    cmake_minimum_required(VERSION 3.13)

    # switch python executable
    if(WIN32)
        set(PYTHON python)
    else()
        set(PYTHON python3)
    endif()

    # Enable exporting compile commands for IntelliSense
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

    # initialize the SDK based on PICO_SDK_PATH
    # note: this must happen before project()
    include(pico_sdk_import.cmake)

### start project
project(ETA-klocka)

    # find source files
    file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS src/**.c src/**.cpp src/**.h src/**.hpp) #PLZ use GLOB_RECURSE not GLOB or the linker will be annoyed
    message(STATUS "Included source files: \n${SRC_FILES}, and the ones generated from images.\n\n")

    # find images
    file(GLOB_RECURSE XBM_FILES CONFIGURE_DEPENDS images/**.xbm)
    message(STATUS "Included image files: \n${XBM_FILES}\n\n")

    # add build action to images
    foreach(XBM_FILE ${XBM_FILES})
        get_filename_component(XBM_NAME ${XBM_FILE} NAME_WE)
        set(GENERATED_FILE "build/generated/images/${XBM_NAME}.xbm")

        add_custom_command(
            OUTPUT ${GENERATED_FILE}
            COMMAND ${PYTHON} package-image-buffers.py --input ${XBM_FILE} --output ${GENERATED_FILE}
            DEPENDS package-image-buffers.py ${XBM_FILE} 
            COMMENT "Generating ${XBM_FILE}.cpp"
            VERBATIM
        )
        list(APPEND SRC_FILES ${GENERATED_FILE})
    endforeach()

    # initialize the Raspberry Pi Pico SDK
    pico_sdk_init()

    # rest of your project
    add_executable(main)

    # platform specific
    pico_enable_stdio_usb(main ENABLED)   # links standard output stream to usb serial (useful for debugging)
    pico_add_extra_outputs(main)          # output map/bin/hex/uf2 file in addition to ELF.

    # include
    target_sources(main PRIVATE ${SRC_FILES})                                    # C source files
    pico_generate_pio_header(main ${CMAKE_CURRENT_LIST_DIR}/src/driver/lcd.pio)  # PIO assembly source files
    target_link_libraries(main pico_stdlib hardware_pio hardware_dma)            # Add pico_stdlib library which aggregates commonly used features


