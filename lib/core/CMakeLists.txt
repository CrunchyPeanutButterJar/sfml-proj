file(GLOB SOURCES src/*.cpp)

find_package(SFML REQUIRED)
find_package(reflectcpp REQUIRED)

add_library(core STATIC ${SOURCES})
target_include_directories(core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(core PUBLIC utils sfml::sfml reflectcpp::reflectcpp)

find_package(GTest REQUIRED)
enable_testing()

add_executable(reflectcpp_integration_tests tests/reflectcpp_integration_tests.cpp)
target_link_libraries(reflectcpp_integration_tests PRIVATE core GTest::gtest_main)

add_test(NAME integration_tests_with_cppreflect COMMAND reflectcpp_integration_tests WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})