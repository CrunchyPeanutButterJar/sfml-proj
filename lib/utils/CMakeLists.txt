file(GLOB SOURCES src/*.cpp)

find_package(fmt REQUIRED)

add_library(utils STATIC ${SOURCES})
target_include_directories(utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(utils PUBLIC fmt::fmt)
if(WIN32)
target_link_libraries(utils PUBLIC shlwapi)
endif()

find_package(GTest REQUIRED)
enable_testing()

add_executable(utilities_unit_tests tests/utilities_unit_tests.cpp)
target_link_libraries(utilities_unit_tests PRIVATE utils GTest::gtest_main)

add_test(NAME utilities_unit_tests COMMAND utilities_unit_tests WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})