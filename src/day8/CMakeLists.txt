add_executable(day8 day8.cpp)
target_link_libraries(day8 PRIVATE advent_of_code_2022 project_option project_warning fmt::fmt)
target_compile_definitions(day8 PRIVATE INPUT_FILE="${CMAKE_CURRENT_SOURCE_DIR}/day8.txt")
set_property(TARGET day8 PROPERTY BUILD_RPATH "/opt/gcc-11.1.0/lib64")
