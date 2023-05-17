add_executable(day5 day5.cpp)
target_link_libraries(day5 PRIVATE advent_of_code_2022 project_option project_warning fmt::fmt ctre::ctre)
target_compile_definitions(day5 PRIVATE INPUT_FILE="${CMAKE_CURRENT_SOURCE_DIR}/day5.txt")
