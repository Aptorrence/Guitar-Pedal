# Cortex-M7 with FPU configuration

set(CORTEX_M7_COMPILE_OPTIONS
    -mcpu=cortex-m7
    -mthumb
    -mfloat-abi=hard
    -mfpu=fpv5-d16      # Single precision; use fpv5-d16 for double
)

set(CORTEX_M7_LINK_OPTIONS
    -mcpu=cortex-m7
    -mthumb
    -mfloat-abi=hard
    -mfpu=fpv5-d16
)

set(CORE_COMPILE_OPTIONS ${CORTEX_M7_COMPILE_OPTIONS})
set(CORE_LINK_OPTIONS ${CORTEX_M7_LINK_OPTIONS})
set(CORE_NAME "cortex-m7")