add_executable(day11 day11.cpp)
target_link_libraries(day11 PRIVATE advent_of_code_2022 project_option project_warning fmt::fmt)
target_compile_definitions(day11 PRIVATE INPUT_FILE="${CMAKE_CURRENT_SOURCE_DIR}/day11.txt")
