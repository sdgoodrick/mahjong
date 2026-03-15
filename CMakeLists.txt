cmake_minimum_required(VERSION 3.10)
project(Mahjigg)

set(CMAKE_CXX_STANDARD 20	)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(SDL3 REQUIRED)
find_package(SDL3_image REQUIRED)

add_executable(${PROJECT_NAME} main.cpp game.cpp texture.cpp graphics.cpp board.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3 SDL3_image::SDL3_image)
