cmake_minimum_required(VERSION 3.28)

project(raii-utils 
  VERSION 1.0.0
  LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra -Wpedantic -Werror)

set(EXPORT_NAMESPACE "${PROJECT_NAME}::")

add_library(${PROJECT_NAME} INTERFACE)
add_library(${EXPORT_NAMESPACE}${PROJECT_NAME} ALIAS ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} INTERFACE include)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  set(IS_TOPLEVEL_PROJECT TRUE)
else()
  set(IS_TOPLEVEL_PROJECT FALSE)
endif()

set(OPTION_PREFIX RAII_UTILS_)

option(${OPTION_PREFIX}BUILD_EXAMPLES "Build examples" ${IS_TOPLEVEL_PROJECT})
option(${OPTION_PREFIX}BUILD_TESTS "Build tests" ${IS_TOPLEVEL_PROJECT})

if(${OPTION_PREFIX}BUILD_EXAMPLES)
  add_subdirectory(example)
endif()

if(${OPTION_PREFIX}BUILD_TESTS)
  enable_testing()
  add_subdirectory(test)
endif()
