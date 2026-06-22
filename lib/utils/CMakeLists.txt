file(GLOB SOURCES src/*.cpp)


find_package(reflectcpp REQUIRED)
find_package(fmt REQUIRED)

add_library(utils STATIC ${SOURCES})
target_include_directories(utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(utils PUBLIC fmt::fmt reflectcpp::reflectcpp)

find_package(GTest REQUIRED)
enable_testing()

add_executable(reflectcpp_integration_tests tests/reflectcpp_integration_tests.cpp)
target_link_libraries(reflectcpp_integration_tests PRIVATE core GTest::gtest_main)

add_executable(utilities_unit_tests tests/utilities_unit_tests.cpp)
target_link_libraries(utilities_unit_tests PRIVATE core GTest::gtest_main)

add_test(NAME integration_tests_with_cppreflect COMMAND reflectcpp_integration_tests WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
add_test(NAME utilities_unit_tests COMMAND utilities_unit_tests WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})