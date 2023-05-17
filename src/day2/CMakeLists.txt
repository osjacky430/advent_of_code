add_executable(day2 day2.cpp)
target_link_libraries(day2 PRIVATE advent_of_code_2022 project_option project_warning fmt::fmt)
target_compile_definitions(day2 PRIVATE INPUT_FILE="${CMAKE_CURRENT_SOURCE_DIR}/day2.txt")
