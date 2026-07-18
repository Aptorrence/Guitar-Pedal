set(EXECUTABLE blink)

# Define Firmware project. app 
add_executable(${EXECUTABLE}
    main.cpp
)

target_link_libraries(${EXECUTABLE} PRIVATE bsp)

add_custom_command(TARGET ${EXECUTABLE} POST_BUILD
  COMMAND ${CMAKE_OBJCOPY} -O ihex   $<TARGET_FILE:${EXECUTABLE}> ${EXECUTABLE}.hex
  COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${EXECUTABLE}> ${EXECUTABLE}.bin
  COMMAND ${CMAKE_SIZE}    --format=berkeley $<TARGET_FILE:${EXECUTABLE}>
  COMMENT "Generated ${EXECUTABLE}.hex and ${EXECUTABLE}.bin"
)
