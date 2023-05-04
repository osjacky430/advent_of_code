add_executable(day25 day25.cpp)
target_link_libraries(day25 PRIVATE advent_of_code_2022 project_option project_warning fmt::fmt)
target_compile_definitions(day25 PRIVATE INPUT_FILE="${CMAKE_CURRENT_SOURCE_DIR}/day25.txt")
set_property(TARGET day25 PROPERTY BUILD_RPATH "/opt/gcc-11.1.0/lib64")
