add_executable(day10 day10.cpp)
target_link_libraries(day10 PRIVATE advent_of_code_2022 project_option project_warning fmt::fmt)
target_compile_definitions(day10 PRIVATE INPUT_FILE="${CMAKE_CURRENT_SOURCE_DIR}/day10.txt")
set_property(TARGET day10 PROPERTY BUILD_RPATH "/opt/gcc-11.1.0/lib64")
