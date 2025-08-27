cmake_minimum_required(VERSION 3.28)
project(moderna_generic VERSION 1.0.0)
set (CMAKE_CXX_STANDARD 23)
set (CMAKE_CXX_STANDARD_REQUIRED true)

option (MODERNA_GENERIC_BUILD_TESTS "Build tests" OFF)
option (MODERNA_GENERIC_MODULES "Build for C++20 Modules" ON)
option (MODERNA_GENERIC_BUILD_EXAMPLE "Build Examples" OFF)

if (MODERNA_GENERIC_MODULES)
  set (CMAKE_CXX_SCAN_FOR_MODULES true)
  if (BUILD_SHARED_LIBS)
    set(MODERNA_GENERIC_LIBRARY_TYPE SHARED)
  else()
    set(MODERNA_GENERIC_LIBRARY_TYPE STATIC)
  endif()
else()
  set(MODERNA_GENERIC_LIBRARY_TYPE INTERFACE)
endif()

add_library(${PROJECT_NAME} ${MODERNA_GENERIC_LIBRARY_TYPE})
add_library(moderna::generic ALIAS ${PROJECT_NAME})
file (GLOB ${PROJECT_NAME}_src "${CMAKE_CURRENT_LIST_DIR}/src/*.cc")

if(MODERNA_GENERIC_MODULES)
  target_sources(${PROJECT_NAME}
    PUBLIC
      FILE_SET 
      CXX_MODULES 
      FILES ${${PROJECT_NAME}_src}
  )
  target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_23)
  target_compile_definitions(${PROJECT_NAME} PRIVATE "EXPORT=export" MODERNA_GENERIC_MODULES)
else()
  foreach(fpath ${${PROJECT_NAME}_src})
    get_filename_component(file_name ${fpath} NAME_WLE)
    configure_file(${fpath} ${CMAKE_BINARY_DIR}/include/moderna/generic/${file_name} COPYONLY)
  endforeach()
  configure_file(${CMAKE_CURRENT_LIST_DIR}/include/generic.hpp ${CMAKE_BINARY_DIR}/include/moderna/generic.hpp)
  target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_BINARY_DIR}/include)
  target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_23)
  target_compile_definitions(${PROJECT_NAME} INTERFACE "EXPORT=")
endif()


if (MODERNA_GENERIC_BUILD_TESTS) 
  if (NOT TARGET moderna_test_lib)
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/libs/test-lib)
  endif()
  add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/tests)
endif()

if (MODERNA_INSTALL)
  include(GNUInstallDirs)
  set (MODERNA_COMPONENT_NAME "generic")
  set_property(
    TARGET ${PROJECT_NAME} 
    PROPERTY EXPORT_NAME ${MODERNA_COMPONENT_NAME}
  )
  install (
    TARGETS ${PROJECT_NAME}
    EXPORT moderna
    FILE_SET 
      CXX_MODULES 
      DESTINATION ${CMAKE_INSTALL_PREFIX}/cxx_modules/moderna/${MODERNA_COMPONENT_NAME}
      COMPONENT ${MODERNA_COMPONENT_NAME}_file_set
    CXX_MODULES_BMI 
      COMPONENT ${MODERNA_COMPONENT_NAME}
      DESTINATION ${CMAKE_INSTALL_PREFIX}/cxx_modules/moderna/${MODERNA_COMPONENT_NAME}
  )
  if (NOT MODERNA_GLOBAL_INSTALL)
    install (
      EXPORT moderna
      DESTINATION ${CMAKE_INSTALL_LIBDIR}/moderna
      FILE moderna-config.cmake
      NAMESPACE moderna::
      CXX_MODULES_DIRECTORY cxx_modules/moderna
    )
  endif()
endif()

if (MODERNA_GENERIC_BUILD_EXAMPLE)
  if (MODERNA_GENERIC_MODULES)
    add_executable(${PROJECT_NAME}_key_vector ${CMAKE_CURRENT_LIST_DIR}/examples/key_vector.cc)
    target_link_libraries(${PROJECT_NAME}_key_vector PRIVATE moderna::generic)
  else()
    add_executable(${PROJECT_NAME}_key_vector ${CMAKE_CURRENT_LIST_DIR}/examples/include_key_vector.cc)
    target_link_libraries(${PROJECT_NAME}_key_vector PRIVATE moderna::generic)
  endif()
endif() 