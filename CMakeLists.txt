cmake_minimum_required(VERSION 3.21 FATAL_ERROR)
project(advent_of_code_2022)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" "${CMAKE_BINARY_DIR}")
include(ProjectOption)

# cmake-format: off
configure_project_option(
  WARNINGS
    TARGET project_warning
  LINKER
    TARGET project_option
    LINKER_NAME mold
    LINKER_PATH /usr/local/libexec/mold
  SANITIZER
    TARGET project_option
  IPO
    DISABLE_FOR_CONFIG Debug Coverage
  PROJ_TARGET
    TARGET advent_of_code_2022
    CXX_STD 23
)
# cmake-format: on

find_package(Boost REQUIRED)
find_package(ctre REQUIRED)
find_package(range-v3 REQUIRED)
find_package(fmt REQUIRED)

target_link_libraries(advent_of_code_2022 INTERFACE range-v3::range-v3)
add_subdirectory(src)
