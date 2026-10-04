add_executable(effect_test main.cpp)
target_link_libraries(effect_test PRIVATE dsp)

add_executable(effect_runner effect_runner.cpp)
target_link_libraries(effect_runner PRIVATE dsp)
