cmake_minimum_required(VERSION 3.28 FATAL_ERROR)
project(advent_of_code)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" "${CMAKE_BINARY_DIR}")
include(ProjectOption)

# cmake-format: off
configure_project_option(
  WARNINGS
    TARGET project_warning
  SANITIZER
    TARGET project_option
  IPO
    DISABLE_FOR_CONFIG Debug Coverage
  PROJ_TARGET
    TARGET advent_of_code
)
# cmake-format: on

find_package(Microsoft.GSL CONFIG REQUIRED)
find_package(ctre REQUIRED)
find_package(range-v3 REQUIRED)
find_package(fmt REQUIRED)

target_link_libraries(advent_of_code INTERFACE range-v3::range-v3 Microsoft.GSL::GSL)
add_subdirectory(src)
