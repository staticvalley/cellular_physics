cmake_minimum_required(VERSION 3.10)
project(cellular_physics C)

################################

set(CMAKE_C_STANDARD 11)
find_package(SDL3 REQUIRED)

################################

file(GLOB SOURCES "src/*.c")
add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} SDL3::SDL3)
target_include_directories(${PROJECT_NAME} PRIVATE ${SDL3_INCLUDE_DIRS})

################################

set(CMAKE_BUILD_TYPE Release)
