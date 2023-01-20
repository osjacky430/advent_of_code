add_executable(day14 day14.cpp)
target_link_libraries(day14 PRIVATE advent_of_code_2022 project_option project_warning fmt::fmt)
target_compile_definitions(day14 PRIVATE INPUT_FILE="${CMAKE_CURRENT_SOURCE_DIR}/day14.txt")
set_property(TARGET day14 PROPERTY BUILD_RPATH "/opt/gcc-11.1.0/lib64")
