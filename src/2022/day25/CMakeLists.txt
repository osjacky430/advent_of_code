add_executable(day25 day25.cpp)
target_link_libraries(day25 PRIVATE advent_of_code_2022 project_option project_warning fmt::fmt)
# target_compile_options(day25 PRIVATE -fmodules -fmodule-map-file=${range-v3_INCLUDE_DIRS}/module.modulemap)
target_compile_definitions(day25 PRIVATE INPUT_FILE="${CMAKE_CURRENT_SOURCE_DIR}/day25.txt")
