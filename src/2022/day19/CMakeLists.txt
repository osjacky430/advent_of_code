add_executable(day19 day19.cpp)
target_link_libraries(day19 PRIVATE advent_of_code project_option project_warning fmt::fmt ctre::ctre)
target_compile_definitions(day19 PRIVATE INPUT_FILE="${CMAKE_CURRENT_SOURCE_DIR}/day19.txt")
