add_executable(day8 day8.cpp)
target_link_libraries(day8 PRIVATE advent_of_code project_option project_warning fmt::fmt)
target_compile_definitions(day8 PRIVATE INPUT_FILE="${CMAKE_CURRENT_SOURCE_DIR}/day8.txt")
