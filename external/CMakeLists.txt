if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/CMSIS-DSP/CMakeLists.txt")
    # Built for both targets so the effects run the same kernels natively and on
    # the M7. No SIMD either way: the M7 has no NEON/Helium, the host build is
    # x86 without NEON. We don't use the float16 kernels.
    set(NEON OFF)
    set(HELIUM OFF)
    set(DISABLEFLOAT16 ON)

    if("${BOARD}" STREQUAL "NATIVE")
        # Host build: drop the CMSIS-Core intrinsic dependency and the package
        # install / export rules.
        set(HOST ON)
        set(CMSISDSP_INSTALL OFF)
    else()
        # Firmware: resolve CMSIS-Core headers from the vendored copy (this also
        # disables the install / export path).
        set(CMSISCORE "${CMAKE_SOURCE_DIR}/mcu_support/CMSIS")
    endif()

    add_subdirectory(CMSIS-DSP)

    # -w silences the vendored C against our -Wall -Wextra -Werror baseline.
    target_compile_options(CMSISDSP PRIVATE -O3 -w)
    if(NOT "${BOARD}" STREQUAL "NATIVE")
        # Arch flags live on board_config, not the global flags, so the CMSIS-DSP
        # sources need them passed explicitly.
        target_compile_options(CMSISDSP PRIVATE ${CORE_COMPILE_OPTIONS})
    endif()
else()
    message(FATAL_ERROR "CMSIS-DSP submodule not initialized (the effects depend on it). Run: git submodule update --init")
endif()
