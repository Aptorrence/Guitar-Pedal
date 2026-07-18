# Create an INTERFACE library that carries all board settings
add_library(board_config INTERFACE)

#inlcude the configs for the mcu core
include(cmake/configs/gcc-m7.cmake)

#device and core selection macros
set(BOARD_DEFINES
    TARGET_H743VI
    CORE_CM7
    STM32H743xx              # Device family macro (required by CMSIS/HAL)
    USE_HAL_DRIVER           # If using STM32 HAL library
    #USE_FULL_LL_DRIVER      # If using Low-Level drivers
    HSE_VALUE=24000000UL     # External crystal frequency (board-specific)
    HSI_VALUE=64000000UL     # Taken from HSI of system_stm32h7xx.c
    #LSE_VALUE=32768UL       # Low-speed external crystal    
)

# include cmsis and mcu specific .h files
set(BOARD_INCLUDE_DIRS
  "${CMAKE_SOURCE_DIR}/mcu_support/CMSIS/include"
  "${CMAKE_SOURCE_DIR}/mcu_support/stm32/h7xx/include"
  "${CMAKE_SOURCE_DIR}/mcu_support/stm32/h7xx/h743/include"
  "${CMAKE_SOURCE_DIR}/mcu_support/stm32/h7xx/HAL"
)

# Vendor startup + linker + system file
set(BOARD_STARTUP_ASM "${CMAKE_SOURCE_DIR}/mcu_support/stm32/h7xx/h743/startup_stm32h743xx.s")
set(BOARD_LINKER_SCRIPT "${CMAKE_SOURCE_DIR}/mcu_support/stm32/h7xx/h743/STM32H743VITX_FLASH.ld")
set(BOARD_SYSTEM_C "${CMAKE_SOURCE_DIR}/mcu_support/stm32/h7xx/system_stm32h7xx.c")

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

# startup assembly only — system_stm32h7xx.c is compiled in st_h7_support
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
  st_h7_hal       # must come before st_h7_support: HAL creates demand for SystemCoreClock
  st_h7_support   # satisfies SystemCoreClock, SystemD2Clock, D1CorePrescTable
)

# BSP — shared across all apps built for this board.

add_library(bsp OBJECT
    ${CMAKE_CURRENT_LIST_DIR}/bsp_h743.cpp
    ${CMAKE_CURRENT_LIST_DIR}/stm32h7xx_it.c
    ${BOARD_STARTUP_ASM}
)
target_include_directories(bsp PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(bsp PUBLIC board_config core componets)
