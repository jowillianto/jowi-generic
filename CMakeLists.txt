cmake_minimum_required(VERSION 3.28)
project(moderna_generic)
set (CMAKE_CXX_STANDARD 23)
set (CMAKE_CXX_STANDARD_REQUIRED true)
set (CMAKE_CXX_SCAN_FOR_MODULES true)
set (CMAKE_CXX_COMPILE_FEATURES "-Wall -Werror")
option (MODERNA_GENERIC_BUILD_TESTS "Build tests" OFF)
include (CTest)

if (NOT TARGET ${PROJECT_NAME})
  add_library(${PROJECT_NAME})
  file (
    GLOB ${PROJECT_NAME}_src
    "${CMAKE_CURRENT_LIST_DIR}/src/*.cc"
  )
  target_sources(${PROJECT_NAME}
    PUBLIC
      FILE_SET CXX_MODULES FILES ${${PROJECT_NAME}_src}
  )
  target_include_directories(${PROJECT_NAME}
    PUBLIC
      "${CMAKE_CURRENT_LIST_DIR}/include"
  )
endif()

if (MODERNA_GENERIC_BUILD_TESTS) 
  if (NOT TARGET moderna_test_lib)
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/libs/test-lib)
  endif()
  add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/tests)
endif()