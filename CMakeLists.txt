cmake_minimum_required(VERSION 3.10)
project(sfml-app)

file(GLOB SOURCES src/*.cpp)
add_compile_options(-std=c++20 -Wall)

add_executable(sfml-app ${SOURCES})
target_include_directories(sfml-app PRIVATE ${CMAKE_SOURCE_DIR}/include)

find_package(SFML REQUIRED)
find_package(reflectcpp REQUIRED)

target_link_libraries(sfml-app PRIVATE sfml::sfml reflectcpp::reflectcpp) 

add_custom_command(TARGET sfml-app POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/resources
            $<TARGET_FILE_DIR:sfml-app>/resources
    COMMENT "📁 Copie du dossier resources vers le dossier du binaire")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "🔍 Build en mode Debug")
    add_compile_definitions(DEBUG_MODE)

target_compile_options(sfml-app PRIVATE
    $<$<CONFIG:Debug>:-Wextra -g -DDEBUG_BUILD -O0>
    $<$<CONFIG:Release>:-O3>
) 

endif()