add_executable(day6 day6.cpp)
target_link_libraries(day6 PRIVATE advent_of_code_2022 project_option project_warning fmt::fmt)
target_compile_definitions(day6 PRIVATE INPUT_FILE="${CMAKE_CURRENT_SOURCE_DIR}/day6.txt")
