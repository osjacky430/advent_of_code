add_executable(day21 day21.cpp)
target_link_libraries(day21 PRIVATE advent_of_code_2022 project_option project_warning fmt::fmt)
target_compile_definitions(day21 PRIVATE INPUT_FILE="${CMAKE_CURRENT_SOURCE_DIR}/day21.txt")
