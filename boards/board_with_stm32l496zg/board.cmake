# Create an INTERFACE library that carries all board settings
add_library(board_config INTERFACE)

#inlcude the configs for the mcu core IE: m4f or m7 , this has the CORE_COMPILE_OPTIONS ect.
include(cmake/configs/gcc-m4f.cmake)

#device and core selection macros
set(BOARD_DEFINES
    TARGET_L496VG
    CORE_CM4
    STM32L496xx              # Device family macro (required by CMSIS/HAL)
    USE_HAL_DRIVER           # If using STM32 HAL library
    #USE_FULL_LL_DRIVER      # If using Low-Level drivers
    HSE_VALUE=24000000UL     # External crystal frequency (board-specific)
    HSI_VALUE=16000000UL     # Taken from HSI of system_stm32l4xx.c
    #LSE_VALUE=32768UL       # Low-speed external crystal            
)

#hi im maybe gonna change the stuff below
# include cmsis and mcu specific .h files
set(BOARD_INCLUDE_DIRS
  "${CMAKE_SOURCE_DIR}/mcu_support/CMSIS/Include"
  "${CMAKE_SOURCE_DIR}/mcu_support/stm32/l4xx/include"
  "${CMAKE_SOURCE_DIR}/mcu_support/stm32/l4xx/l496/include"
)

# Vendor startup + linker + system file
set(BOARD_STARTUP_ASM "${CMAKE_SOURCE_DIR}/mcu_support/stm32/l4xx/l496/startup_stm32l496xx.s")
set(BOARD_LINKER_SCRIPT "${CMAKE_SOURCE_DIR}/mcu_support/stm32/l4xx/l496/STM32L496ZGTX_FLASH.ld")

# Include directories
target_include_directories(board_config SYSTEM INTERFACE ${BOARD_INCLUDE_DIRS})

# Compile definitions (macros like STM32L496xx)
target_compile_definitions(board_config INTERFACE ${BOARD_DEFINES})

# Compile options (CPU/FPU flags)
target_compile_options(board_config INTERFACE ${CORE_COMPILE_OPTIONS})

# Link options (linker script, CPU flags, specs)
target_link_options(board_config INTERFACE
  ${CORE_LINK_OPTIONS}
  "-T${BOARD_LINKER_SCRIPT}"     # memory layout from vendor script
  --specs=nosys.specs            # no hose syscalls; stubs are provided
  -Wl,--gc-sections              # remove unused sections (works with functions/data-sections)
  -Wl,--start-group -lc -lm -lgcc -Wl,--end-group  # ensure C runtime, math, and compiler support libs link
)

# startup/system sources create an OBJECT library
add_library(board_startup OBJECT
  ${BOARD_STARTUP_ASM}
)

# Carries device includes/defines/CPU flags into the vendor HAL build
add_library(board_hal_config INTERFACE)
target_include_directories(board_hal_config SYSTEM INTERFACE ${BOARD_INCLUDE_DIRS})
target_compile_definitions(board_hal_config INTERFACE ${BOARD_DEFINES})
target_compile_options(board_hal_config INTERFACE ${CORE_COMPILE_OPTIONS})

# linking the startup/system to the board_configs
target_link_libraries(board_config INTERFACE
  board_startup
  st_l4_support
)

# BSP — shared across all apps built for this board
add_library(bsp STATIC
    ${CMAKE_CURRENT_LIST_DIR}/bsp_l496.cpp
    $<TARGET_OBJECTS:board_startup>
)
target_include_directories(bsp PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(bsp PUBLIC board_config core)

 