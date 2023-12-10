add_executable(day20 day20.cpp)
target_link_libraries(day20 PRIVATE advent_of_code project_option project_warning fmt::fmt)
target_compile_definitions(day20 PRIVATE INPUT_FILE="${CMAKE_CURRENT_SOURCE_DIR}/day20.txt")
