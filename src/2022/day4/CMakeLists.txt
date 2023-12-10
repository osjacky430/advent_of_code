add_executable(day4 day4.cpp)
target_link_libraries(day4 PRIVATE advent_of_code project_option project_warning fmt::fmt)
target_compile_definitions(day4 PRIVATE INPUT_FILE="${CMAKE_CURRENT_SOURCE_DIR}/day4.txt")
