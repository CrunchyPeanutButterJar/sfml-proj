cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 23)

set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

project(sfml-app)

if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "18.0")
  message(FATAL_ERROR "Clang 18 requis pour ce projet.")
endif()

set(MAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")
file(GLOB SOURCES src/*.cpp)

list(REMOVE_ITEM SOURCES "${MAIN_FILE}")

add_library(core STATIC ${SOURCES})
target_include_directories(core PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(core PUBLIC sfml::sfml reflectcpp::reflectcpp fmt::fmt)

target_compile_options(core PUBLIC
  -std=c++23
  -stdlib=libc++
  -Wall
  -Werror
  -Wextra
)

target_link_options(core PUBLIC
  -lc++
  -stdlib=libc++
  -lc++abi
)

add_executable(sfml-app ${MAIN_FILE}) 

find_package(SFML REQUIRED)
find_package(reflectcpp REQUIRED)
find_package(fmt REQUIRED)

target_link_libraries(sfml-app PRIVATE core) 

add_custom_command(TARGET sfml-app POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/resources
            $<TARGET_FILE_DIR:sfml-app>/resources
    COMMENT "📁 Copie du dossier resources vers le dossier du binaire")

add_custom_command(TARGET sfml-app POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/config
            $<TARGET_FILE_DIR:sfml-app>/config
    COMMENT "📁 Copie du dossier config vers le dossier du binaire")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "🔍 Build en mode Debug")
    add_compile_definitions(DEBUG_MODE)
endif()

target_compile_options(sfml-app PRIVATE
    $<$<CONFIG:Debug>:-g -DDEBUG_BUILD -O0>
    $<$<CONFIG:Release>:-O3>
) 

find_package(GTest REQUIRED)
enable_testing()

add_executable(reflectcpp_integration_tests tests/reflectcpp_integration_tests.cpp)
target_link_libraries(reflectcpp_integration_tests PRIVATE core GTest::gtest_main)

add_executable(utilities_unit_tests tests/utilities_unit_tests.cpp)
target_link_libraries(utilities_unit_tests PRIVATE core GTest::gtest_main)

add_test(NAME integration_tests_with_cppreflect COMMAND reflectcpp_integration_tests WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
add_test(NAME utilities_unit_tests COMMAND utilities_unit_tests WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})