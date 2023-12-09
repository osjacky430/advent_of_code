add_executable(2023_day2 day2.cpp)
target_link_libraries(2023_day2 PRIVATE advent_of_code_2022 project_option project_warning fmt::fmt)
target_compile_definitions(2023_day2 PRIVATE INPUT_FILE="${CMAKE_CURRENT_SOURCE_DIR}/day2.txt")
