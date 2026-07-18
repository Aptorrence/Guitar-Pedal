# Helper to apply common firmware target settings
function(configure_firmware_target TARGET)
    cmake_parse_arguments(ARG
        "NO_DEFAULT_LIBS"                           # Options
        "LINKER_SCRIPT"                             # Single-value
        "EXTRA_DEFINITIONS;EXTRA_OPTIONS"           # Multi-value
        ${ARGN}
    )

    # Apply board-level settings
    target_compile_definitions(${TARGET}
        PRIVATE
            ${BOARD_DEFINES}
            ${ARG_EXTRA_DEFINITIONS}
    )

    target_compile_options(${TARGET}
        PRIVATE
            ${CORE_COMPILE_OPTIONS}
            -Wall -Wextra
            $<$<CONFIG:Debug>:${CORE_COMPILE_OPTIONS_DEBUG}>
            $<$<CONFIG:Release>:${CORE_COMPILE_OPTIONS_RELEASE}>
            ${ARG_EXTRA_OPTIONS}
    )

    target_include_directories(${TARGET}
        PRIVATE
            ${BOARD_INCLUDE_DIRS}
    )

    # Linker configuration
    if(ARG_LINKER_SCRIPT)
        set(_linker_script ${ARG_LINKER_SCRIPT})
    else()
        set(_linker_script ${BOARD_LINKER_SCRIPT})
    endif()

    target_link_options(${TARGET}
        PRIVATE
            ${CORE_LINK_OPTIONS}
            -T${_linker_script}
            -Wl,-Map=${ARTIFACT_DIR}/${TARGET}.map,--cref
            -Wl,--gc-sections
            --specs=nosys.specs
    )

    if(NOT ARG_NO_DEFAULT_LIBS)
        target_link_options(${TARGET}
            PRIVATE
                -Wl,--start-group -lc -lm -lgcc -Wl,--end-group
        )
    endif()

    # Output location
    set_target_properties(${TARGET} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${ARTIFACT_DIR}"
    )

    # Post-build artifacts
    add_custom_command(TARGET ${TARGET} POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${TARGET}> ${ARTIFACT_DIR}/${TARGET}.hex
        COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${TARGET}> ${ARTIFACT_DIR}/${TARGET}.bin
        COMMAND ${CMAKE_SIZE} --format=berkeley $<TARGET_FILE:${TARGET}>
        COMMENT "Generated ${TARGET}.hex and ${TARGET}.bin"
    )
endfunction()
