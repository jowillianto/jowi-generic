cmake_minimum_required(VERSION 3.28)
project(jowi_generic VERSION 1.0.0)
set (CMAKE_CXX_STANDARD 23)
set (CMAKE_CXX_STANDARD_REQUIRED true)
set (CMAKE_CXX_SCAN_FOR_MODULES true)

option (JOWI_GENERIC_BUILD_TESTS "Build tests" OFF)
option (JOWI_GENERIC_CONSTEXPR_TESTS "Run constexpr tests" OFF)

add_library(${PROJECT_NAME})
add_library(jowi::generic ALIAS ${PROJECT_NAME})
target_sources(${PROJECT_NAME}
  PUBLIC
    FILE_SET CXX_MODULES
    FILES
        ${CMAKE_CURRENT_LIST_DIR}/src/fixed_string.cc
        ${CMAKE_CURRENT_LIST_DIR}/src/is_formattable_error.cc
        ${CMAKE_CURRENT_LIST_DIR}/src/key_vector.cc
        ${CMAKE_CURRENT_LIST_DIR}/src/main.cc
        ${CMAKE_CURRENT_LIST_DIR}/src/unique_handle.cc
        ${CMAKE_CURRENT_LIST_DIR}/src/atomic.cc
        ${CMAKE_CURRENT_LIST_DIR}/src/variant.cc
)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_23)

if (JOWI_GENERIC_CONSTEXPR_TESTS)
    target_compile_definitions(${PROJECT_NAME}
    PRIVATE
      JOWI_GENERIC_CONSTEXPR_TESTS
  )
endif()

if (JOWI_GENERIC_BUILD_TESTS)
    include (CTest)
    if (NOT TARGET moderna_test_lib)
        add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/libs/test-lib)
    endif()
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/tests)
endif()

if (MODERNA_INSTALL)
    include(GNUInstallDirs)
    set (JOWI_COMPONENT_NAME "generic")
    set_property(
    TARGET ${PROJECT_NAME}
    PROPERTY EXPORT_NAME ${JOWI_COMPONENT_NAME}
  )
    install (
    TARGETS ${PROJECT_NAME}
    EXPORT jowi
    FILE_SET
      CXX_MODULES
      DESTINATION ${CMAKE_INSTALL_PREFIX}/cxx_modules/jowi/${JOWI_COMPONENT_NAME}
      COMPONENT ${JOWI_COMPONENT_NAME}_file_set
    CXX_MODULES_BMI
      COMPONENT ${JOWI_COMPONENT_NAME}
      DESTINATION ${CMAKE_INSTALL_PREFIX}/cxx_modules/jowi/${JOWI_COMPONENT_NAME}
  )
    if (NOT MODERNA_GLOBAL_INSTALL)
        install (
      EXPORT jowi
      DESTINATION ${CMAKE_INSTALL_LIBDIR}/jowi
      FILE jowi-config.cmake
      NAMESPACE jowi::
      CXX_MODULES_DIRECTORY cxx_modules/jowi
    )
    endif()
endif()
