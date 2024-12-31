cmake_minimum_required(VERSION 3.30)
project(k_nearest_neighbors)

set(CMAKE_CXX_STANDARD 26)

add_executable(k_nearest_neighbors main.cpp)
target_link_libraries(k_nearest_neighbors PUBLIC plplot)