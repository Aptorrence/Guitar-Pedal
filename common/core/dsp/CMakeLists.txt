add_library(dsp STATIC
    arm_backend.cpp
)

target_include_directories(dsp PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# CMSIS-DSP backs the effect primitives on both targets (see external/CMakeLists.txt).
target_link_libraries(dsp PUBLIC CMSISDSP)

if(NOT "${BOARD}" STREQUAL "NATIVE")
    # Arch flags live on board_config, not the global flags.
    target_compile_options(dsp PRIVATE ${CORE_COMPILE_OPTIONS})
endif()
