file(GLOB SOURCES src/*.cpp)

find_package(SFML REQUIRED)

add_library(ecs STATIC ${SOURCES})
target_include_directories(ecs PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(ecs PUBLIC utils sfml::sfml core)