cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 23)

project(sfml-app)

if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "18.0")
  message(FATAL_ERROR "Clang 18 requis pour ce projet.")
endif()

add_compile_options(
    -std=c++23
    -stdlib=libc++
    -Wall
    -Werror
    -Wextra
    $<$<CONFIG:Debug>:-g>
    $<$<CONFIG:Debug>:-DDEBUG_BUILD>
    $<$<CONFIG:Debug>:-O0>
    $<$<CONFIG:Debug>:-fsanitize=address>
    $<$<CONFIG:Debug>:-fno-omit-frame-pointer>
    $<$<CONFIG:Release>:-O3>
)

add_link_options(
  -lc++
  -stdlib=libc++
  -lc++abi
  $<$<CONFIG:Debug>:-g>
  $<$<CONFIG:Debug>:-fsanitize=address>
  $<$<CONFIG:Debug>:-fno-omit-frame-pointer>
)

add_subdirectory(lib/utils)
add_subdirectory(lib/core)
add_subdirectory(lib/ecs)

set(MAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")
file(GLOB SOURCES src/*.cpp)

list(REMOVE_ITEM SOURCES "${MAIN_FILE}")

add_library(app STATIC ${SOURCES})
target_include_directories(app PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(app PUBLIC ecs)

add_executable(sfml-app ${MAIN_FILE}) 

target_link_libraries(sfml-app PRIVATE app) 

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
endif()

enable_testing()