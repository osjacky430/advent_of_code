add_executable(day23 day23.cpp)
target_link_libraries(day23 PRIVATE advent_of_code_2022 project_option project_warning fmt::fmt)
target_compile_definitions(day23 PRIVATE INPUT_FILE="${CMAKE_CURRENT_SOURCE_DIR}/day23.txt")
