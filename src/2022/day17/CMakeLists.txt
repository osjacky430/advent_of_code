add_executable(day17 day17.cpp)
target_link_libraries(day17 PRIVATE advent_of_code project_option project_warning fmt::fmt)
target_compile_definitions(day17 PRIVATE INPUT_FILE="${CMAKE_CURRENT_SOURCE_DIR}/day17.txt")
