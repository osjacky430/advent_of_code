add_executable(day13 day13.cpp)
target_link_libraries(day13 PRIVATE advent_of_code_2022 project_option project_warning fmt::fmt)
target_compile_definitions(day13 PRIVATE INPUT_FILE="${CMAKE_CURRENT_SOURCE_DIR}/day13.txt")
set_property(TARGET day13 PROPERTY BUILD_RPATH "/opt/gcc-11.1.0/lib64")
