add_executable(day12 day12.cpp)
target_link_libraries(day12 PRIVATE advent_of_code project_option project_warning fmt::fmt)
target_compile_definitions(day12 PRIVATE INPUT_FILE="${CMAKE_CURRENT_SOURCE_DIR}/day12.txt")
