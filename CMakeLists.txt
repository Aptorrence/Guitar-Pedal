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

#4 Load board overlay
if("${BOARD}" STREQUAL "dsp_pedal")
    include(boards/dsp_pedal/board.cmake)
elseif("${BOARD}" STREQUAL "board_with_stm32l496zg")
    include(boards/board_with_stm32l496zg/board.cmake)
elseif("${BOARD}" STREQUAL "NATIVE")
    include(boards/native/board.cmake)
endif()

#5 if not running nativly need, toolchain, emit .elf file, and required cache varibles
if(NOT "${BOARD}" STREQUAL "NATIVE")

    #6 Include toolchain and helper modules
    #include(cmake/modules/build-util.cmake) #not used right now
    include(cmake/toolchains/arm-gcc.cmake)

    #7 emit .elf files
    set(CMAKE_EXECUTABLE_SUFFIX ".elf")

    #8 check for required cache varibles
    foreach(req IN ITEMS
      #from CmakePresets.json
      BOARD
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
endif()

#9 Add subdirectories
add_subdirectory(mcu_support)
add_subdirectory(common)
add_subdirectory(external)

#10 Build specific app or all apps
if(DEFINED TARGET_APP)
    add_subdirectory(app/${TARGET_APP})
else()
    add_subdirectory(app)
endif()