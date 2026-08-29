cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(sfml-app)

if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "18.0")
  message(FATAL_ERROR "Clang 18 requis pour ce projet.")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(WIN32)
add_compile_definitions(NOMINMAX)
endif()

add_compile_options(
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wextra>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Werror>
)

add_compile_options(
    $<$<CONFIG:Debug>:-DDEBUG_BUILD>
    $<$<AND:$<CONFIG:Debug>,$<NOT:$<CXX_COMPILER_ID:MSVC>>>:-g>
    $<$<AND:$<CONFIG:Debug>,$<NOT:$<CXX_COMPILER_ID:MSVC>>>:-O0>
    $<$<AND:$<CONFIG:Debug>,$<NOT:$<CXX_COMPILER_ID:MSVC>>>:-fsanitize=address>
    $<$<AND:$<CONFIG:Debug>,$<NOT:$<CXX_COMPILER_ID:MSVC>>>:-fno-omit-frame-pointer>
    $<$<AND:$<CONFIG:Debug>,$<CXX_COMPILER_ID:MSVC>>:/Zi>
    $<$<AND:$<CONFIG:Debug>,$<CXX_COMPILER_ID:MSVC>>:/Od>
    $<$<AND:$<CONFIG:Debug>,$<CXX_COMPILER_ID:MSVC>>:/fsanitize=address>
)

add_link_options(
    $<$<AND:$<CONFIG:Debug>,$<NOT:$<CXX_COMPILER_ID:MSVC>>>:-g>
    $<$<AND:$<CONFIG:Debug>,$<NOT:$<CXX_COMPILER_ID:MSVC>>>:-fsanitize=address>
    $<$<AND:$<CONFIG:Debug>,$<NOT:$<CXX_COMPILER_ID:MSVC>>>:-fno-omit-frame-pointer>
    $<$<AND:$<CONFIG:Debug>,$<CXX_COMPILER_ID:MSVC>>:/DEBUG>
    $<$<AND:$<CONFIG:Debug>,$<CXX_COMPILER_ID:MSVC>>:/fsanitize=address>
    $<$<AND:$<CONFIG:Debug>,$<CXX_COMPILER_ID:MSVC>>:/INCREMENTAL:NO>
)

add_compile_options(
    $<$<AND:$<CONFIG:Release>,$<NOT:$<CXX_COMPILER_ID:MSVC>>>:-O3>
    $<$<AND:$<CONFIG:Release>,$<CXX_COMPILER_ID:MSVC>>:/O2>
)

if(UNIX)
add_compile_options(
    -stdlib=libc++
)
endif()


if(UNIX)
add_link_options(
  -lc++
  -stdlib=libc++
  -lc++abi
)
endif()

function(enable_project_pch target)
    target_precompile_headers(${target} PRIVATE
        <vector>
        <string>
        <memory>
    )
endfunction()

add_subdirectory(lib/utils)
add_subdirectory(lib/core)
add_subdirectory(lib/ecs)

enable_project_pch(utils)
enable_project_pch(core)
enable_project_pch(ecs)

set(MAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")
file(GLOB SOURCES src/*.cpp)

list(REMOVE_ITEM SOURCES "${MAIN_FILE}")

add_library(app STATIC ${SOURCES})

enable_project_pch(app)

target_include_directories(app PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(app PUBLIC ecs)

add_executable(sfml-app ${MAIN_FILE}) 

enable_project_pch(sfml-app)

target_link_libraries(sfml-app PRIVATE app) 

add_custom_target(print_config ALL
    COMMAND ${CMAKE_COMMAND} -E echo "🔍 Building in $<CONFIG> mode"
)

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

enable_testing()