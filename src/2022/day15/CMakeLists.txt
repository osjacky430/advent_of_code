add_executable(day15 day15.cpp)
target_link_libraries(day15 PRIVATE advent_of_code project_option project_warning fmt::fmt ctre::ctre)
target_compile_definitions(day15 PRIVATE INPUT_FILE="${CMAKE_CURRENT_SOURCE_DIR}/day15.txt")
