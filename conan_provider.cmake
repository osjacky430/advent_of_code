cmake_minimum_required(VERSION 3.25 FATAL_ERROR)

if (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/conan_support.cmake)
  file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/develop2/conan_support.cmake"
       "${CMAKE_CURRENT_SOURCE_DIR}/conan_support.cmake")
endif ()

include(${CMAKE_CURRENT_SOURCE_DIR}/conan_support.cmake)

cmake_language(SET_DEPENDENCY_PROVIDER conan_provide_dependency SUPPORTED_METHODS FIND_PACKAGE)
