add_executable(day10 day10.cpp)
target_link_libraries(day10 PRIVATE advent_of_code project_option project_warning fmt::fmt)
target_compile_definitions(day10 PRIVATE INPUT_FILE="${CMAKE_CURRENT_SOURCE_DIR}/day10.txt")
