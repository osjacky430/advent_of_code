add_executable(day18 day18.cpp)
target_link_libraries(day18 PRIVATE advent_of_code_2022 project_option project_warning fmt::fmt)
target_compile_definitions(day18 PRIVATE INPUT_FILE="${CMAKE_CURRENT_SOURCE_DIR}/day18.txt")
set_property(TARGET day18 PROPERTY BUILD_RPATH "/opt/gcc-11.1.0/lib64")
