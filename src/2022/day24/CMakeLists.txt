add_executable(day24 day24.cpp)
target_link_libraries(day24 PRIVATE advent_of_code project_option project_warning fmt::fmt)
target_compile_definitions(day24 PRIVATE INPUT_FILE="${CMAKE_CURRENT_SOURCE_DIR}/day24.txt")
