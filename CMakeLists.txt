cmake_minimum_required(VERSION 3.10)
project(sfml-app)

file(GLOB SOURCES src/*.cpp)
add_compile_options(-std=c++20 -Wall -O3)

add_executable(sfml-app ${SOURCES})
target_include_directories(sfml-app PRIVATE ${CMAKE_SOURCE_DIR}/include)

find_package(SFML REQUIRED)
find_package(reflectcpp REQUIRED)

target_link_libraries(sfml-app PRIVATE sfml::sfml reflectcpp::reflectcpp) 
