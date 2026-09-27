#1 Define minimum CMake and project name and languages
cmake_minimum_required(VERSION 3.27)


#1.a) Detect a moved or renamed source tree and give freindly error (optional)
set(CACHED_SOURCE_DIR "${CMAKE_SOURCE_DIR}" CACHE INTERNAL "Configured source dir")
if(NOT CACHED_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
 message(FATAL_ERROR
    "This build directory was generator for :\n ${CACHED_SOURCE_DIR}\n"
    "but the source is now at \n"${CMAKE_SOURCE_DIR}""
    "Delete the 'build folder and reconfigure(use the presets).")
endif()

#2) Declare project name and progamming language for compliation and C++ 20 standard need for span
project(myapp LANGUAGES CXX C ASM)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#2.a) Forbid in-source builds (keep source tree clean by keep generated files separate)
iF(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR "In-source builds are disallowed. Configure with a separate build directory")
endif()


#3 Require BOARD to be set. comes from CmakePresets.json 
if(NOT DEFINED BOARD)
    message(FATAL_ERROR "preset must be defined. Use cmake --list-presets=build to see options" )
endif()

#4 Load board overlay (boards/<board>/board.cmake; BOARD=NATIVE maps to boards/native)
string(TOLOWER "${BOARD}" BOARD_DIR)
include(boards/${BOARD_DIR}/board.cmake)

#5 if not running nativly need, toolchain, emit .elf file, and required cache varibles
if(NOT "${BOARD}" STREQUAL "NATIVE")

    #6 Include toolchain
    include(cmake/toolchains/arm-gcc.cmake)

    #7 emit .elf files
    set(CMAKE_EXECUTABLE_SUFFIX ".elf")

    #8 check for required cache varibles
    foreach(req IN ITEMS
      #from CmakePresets.json
      MCU_NAME
      #from board.cmake
      BOARD_DEFINES
      BOARD_INCLUDE_DIRS
      BOARD_STARTUP_ASM
      BOARD_LINKER_SCRIPT
      #from gcc-mx.cmake
      CORE_COMPILE_OPTIONS
      CORE_LINK_OPTIONS
      )

    if(NOT DEFINED ${req})
        message(FATAL_ERROR "Board overlay for ${BOARD} did not define ${req}")
    endif()

    endforeach()

    #8.a Emit .hex/.bin next to a firmware .elf and print its size
    function(add_firmware_outputs TARGET)
        add_custom_command(TARGET ${TARGET} POST_BUILD
          COMMAND ${CMAKE_OBJCOPY} -O ihex   $<TARGET_FILE:${TARGET}> ${TARGET}.hex
          COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${TARGET}> ${TARGET}.bin
          COMMAND ${CMAKE_SIZE}    --format=berkeley $<TARGET_FILE:${TARGET}>
          COMMENT "Generated ${TARGET}.hex and ${TARGET}.bin"
        )
    endfunction()
endif()

#9 Add subdirectories (external before common: common/core/dsp links CMSISDSP)
add_subdirectory(mcu_support)
add_subdirectory(external)
add_subdirectory(common)

#10 Build specific app or all apps
if(DEFINED TARGET_APP)
    add_subdirectory(app/${TARGET_APP})
else()
    add_subdirectory(app)
endif()