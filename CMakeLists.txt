cmake_minimum_required(VERSION 3.3)

project(handmade)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})
set(CMAKE_CXX_FLAGS_DEBUG "-DDEBUG")
set(LIBS_ROOT "${CMAKE_BINARY_DIR}/libs")
add_definitions(-DLIBS_DIR=${LIBS_ROOT})

add_definitions(-std=c++11)
set(CMAKE_CXX_FLAGS "-O2")

add_subdirectory(libs)
add_subdirectory(game_loop)
