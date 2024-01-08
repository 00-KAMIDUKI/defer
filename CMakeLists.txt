cmake_minimum_required(VERSION 3.28)

project(defer 
  VERSION 1.0.0
  LANGUAGES CXX
)

add_library(defer INTERFACE)
add_library(defer::defer ALIAS defer)
target_include_directories(defer INTERFACE include)

