function(add_example name)
  add_executable(${name} ${name}.cc)
  target_link_libraries(${name} ${PROJECT_NAME})
endfunction()

add_example(example)
