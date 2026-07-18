# Cortex-M4 with FPU configuration
# Include this in your board.cmake 

set(CORTEX_M4F_COMPILE_OPTIONS
    -mcpu=cortex-m4
    -mthumb
    -mfloat-abi=hard
    -mfpu=fpv4-sp-d16
)

set(CORTEX_M4F_LINK_OPTIONS
    -mcpu=cortex-m4
    -mthumb
    -mfloat-abi=hard
    -mfpu=fpv4-sp-d16
)

# Export as CORE_* for board overlays
set(CORE_COMPILE_OPTIONS ${CORTEX_M4F_COMPILE_OPTIONS})
set(CORE_LINK_OPTIONS ${CORTEX_M4F_LINK_OPTIONS})
set(CORE_NAME "cortex-m4f")