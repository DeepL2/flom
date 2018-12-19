cmake_minimum_required(VERSION 3.0.2)

function(flom_add_test target)
  target_link_libraries(${target} PRIVATE flom_lib rapidcheck)
  if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(${target} PUBLIC -Wno-global-constructors -Wno-disabled-macro-expansion)
  endif()
  add_test(NAME ${target} COMMAND ${target})
  enable_clang_format(${target})
  enable_clang_tidy(${target})
endfunction()