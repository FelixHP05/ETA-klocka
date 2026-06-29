cmake_minimum_required(VERSION 3.13)

    # include external CMake
    # note: this must happen before project()
    include(pico_sdk_import.cmake)

    # set variables
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)   # Enable exporting compile commands for IntelliSense
    if(WIN32)                               # switch python executable
        set(PYTHON python)
    else()
        set(PYTHON python3)
    endif()

### start project
project(ETA-klocka)


    # init
    add_executable(main)
    pico_sdk_init()
    pico_enable_stdio_usb(main ENABLED)   # links standard output stream to usb serial (useful for debugging)
    message("\n")                         # add space after pico_sdk_init
    

    # find source files
    file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS src/*.c src/*.cpp src/*.h src/*.hpp) #PLZ use GLOB_RECURSE not GLOB or the linker will be annoyed
    message(STATUS "Included source files: \n${SRC_FILES}, and the ones generated from images.\n\n")

    # find images
    file(GLOB_RECURSE XBM_FILES CONFIGURE_DEPENDS images/*.xbm)
    message(STATUS "Included image files: \n${XBM_FILES}\n\n")

    # specify include roots
    set(INCLUDE_DIRS 
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/images
    )
    message(STATUS "Include directories are: \n${INCLUDE_DIRS}")


    # add build rule for .xbm images --> .cpp
    # also adds more SRC_FILES (ofc)
    foreach(XBM_FILE ${XBM_FILES})
        get_filename_component(XBM_NAME ${XBM_FILE} NAME_WE)
        set(GENERATED_FILE "${CMAKE_SOURCE_DIR}/build/generated/images/${XBM_NAME}.cpp")
        list(APPEND SRC_FILES ${GENERATED_FILE})


        add_custom_command(
            OUTPUT ${GENERATED_FILE}
            COMMAND ${PYTHON}
                    ${CMAKE_SOURCE_DIR}/package-image-buffers.py 
                    --input ${XBM_FILE}
                    --output ${GENERATED_FILE}
            DEPENDS package-image-buffers.py ${XBM_FILE} 
            COMMENT "Generating ${XBM_NAME}.cpp"
            VERBATIM
        )
    endforeach()

    # add build rule for PIO lcd 
    pico_generate_pio_header(main ${CMAKE_CURRENT_LIST_DIR}/src/driver/lcd.pio)  # PIO assembly source files

    # add build rule .elf --> .uf2
    pico_add_extra_outputs(main)          # output map/bin/hex/uf2 file in addition to ELF.


    # specify contents of target executable
    # like a high level build rule .cpp --> main.elf
    target_sources(main PRIVATE ${SRC_FILES})                           # C source files
    target_include_directories(main PRIVATE ${INCLUDE_DIRS})            # headers
    target_link_libraries(main pico_stdlib hardware_pio hardware_dma)   # Add pico_stdlib library which aggregates commonly used features

