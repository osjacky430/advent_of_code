add_executable(day16 day16.cpp)
target_link_libraries(day16 PRIVATE advent_of_code_2022 project_option project_warning fmt::fmt ctre::ctre)
target_compile_definitions(day16 PRIVATE INPUT_FILE="${CMAKE_CURRENT_SOURCE_DIR}/day16.txt")
set_property(TARGET day16 PROPERTY BUILD_RPATH "/opt/gcc-11.1.0/lib64")
