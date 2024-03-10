cmake_minimum_required(VERSION 3.28)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(raii-utils 
  VERSION 1.0.0
  LANGUAGES CXX
)

add_library(raii-utils INTERFACE)
add_library(raii-utils::raii-utils ALIAS raii-utils)
target_include_directories(raii-utils INTERFACE include)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  set(IS_TOPLEVEL_PROJECT ON)
else()
  set(IS_TOPLEVEL_PROJECT OFF)
endif()

option(RAII_UTILS_BUILD_EXAMPLES "Build examples" ${IS_TOPLEVEL_PROJECT})
option(RAII_UTILS_BUILD_TESTS    "Build tests"    ${IS_TOPLEVEL_PROJECT})

if(RAII_UTILS_BUILD_EXAMPLES)
  add_subdirectory(example)
endif()

if(RAII_UTILS_BUILD_TESTS)
  enable_testing()
  add_subdirectory(test)
endif()
