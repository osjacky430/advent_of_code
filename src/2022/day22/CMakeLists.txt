add_executable(day22 day22.cpp)
target_link_libraries(day22 PRIVATE advent_of_code_2022 project_option project_warning fmt::fmt)
target_compile_definitions(day22 PRIVATE INPUT_FILE="${CMAKE_CURRENT_SOURCE_DIR}/day22.txt")
