if (NOT "${FREERTOS_PORT}" STREQUAL "")
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/FreeRTOS-Kernel/CMakeLists.txt")
        add_library(freertos_config INTERFACE)
        target_compile_definitions(freertos_config INTERFACE projCOVERAGE_TEST=0)
        target_link_libraries(freertos_config INTERFACE hal)
        add_subdirectory(FreeRTOS-Kernel)
    else()
        message(STATUS "FreeRTOS-Kernel submodule not initialized — skipping. Run: git submodule update --init")
    endif()
endif()

if ("${BOARD}" MATCHES "NATIVE")
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/googletest/CMakeLists.txt")
        # For Windows: Prevent overriding the parent project's compiler/linker settings
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
        add_subdirectory(googletest)
    else()
        message(STATUS "googletest submodule not initialized — skipping. Run: git submodule update --init")
    endif()
endif()

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
