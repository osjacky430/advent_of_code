add_executable(day24 day24.cpp)
target_link_libraries(day24 PRIVATE advent_of_code_2022 project_option project_warning fmt::fmt)
target_compile_definitions(day24 PRIVATE INPUT_FILE="${CMAKE_CURRENT_SOURCE_DIR}/day24.txt")
set_property(TARGET day24 PROPERTY BUILD_RPATH "/opt/gcc-11.1.0/lib64")
