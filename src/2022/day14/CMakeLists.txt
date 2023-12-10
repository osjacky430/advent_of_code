add_executable(day14 day14.cpp)
target_link_libraries(day14 PRIVATE advent_of_code project_option project_warning fmt::fmt)
target_compile_definitions(day14 PRIVATE INPUT_FILE="${CMAKE_CURRENT_SOURCE_DIR}/day14.txt")
