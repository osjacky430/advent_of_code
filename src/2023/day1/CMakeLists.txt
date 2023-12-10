add_executable(2023_day1 day1.cpp)
target_link_libraries(2023_day1 PRIVATE advent_of_code project_option project_warning fmt::fmt)
target_compile_definitions(2023_day1 PRIVATE INPUT_FILE="${CMAKE_CURRENT_SOURCE_DIR}/day1.txt")
