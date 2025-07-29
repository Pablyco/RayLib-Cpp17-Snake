cmake_minimum_required(VERSION 3.15)
project(RayLibSnake)

set(CMAKE_CXX_STANDARD 17)

set(RAYLIB_PATH "${CMAKE_SOURCE_DIR}/raylib")

include_directories(${RAYLIB_PATH}/include)
link_directories(${RAYLIB_PATH}/lib)

add_executable(RayLibSnake main.cpp)

target_link_libraries(RayLibSnake raylib gdi32 winmm)
