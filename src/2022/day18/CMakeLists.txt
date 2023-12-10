add_executable(day18 day18.cpp)
target_link_libraries(day18 PRIVATE advent_of_code project_option project_warning fmt::fmt)
target_compile_definitions(day18 PRIVATE INPUT_FILE="${CMAKE_CURRENT_SOURCE_DIR}/day18.txt")
