add_executable(day7 day7.cpp)
target_link_libraries(day7 PRIVATE advent_of_code_2022 project_option project_warning fmt::fmt)
target_compile_definitions(day7 PRIVATE INPUT_FILE="${CMAKE_CURRENT_SOURCE_DIR}/day7.txt")
set_property(TARGET day7 PROPERTY BUILD_RPATH "/opt/gcc-11.1.0/lib64")
