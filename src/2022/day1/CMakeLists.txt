add_executable(day1 day1.cpp)
target_link_libraries(day1 PRIVATE advent_of_code project_option project_warning fmt::fmt)
target_compile_definitions(day1 PRIVATE INPUT_FILE="${CMAKE_CURRENT_SOURCE_DIR}/day1.txt")
