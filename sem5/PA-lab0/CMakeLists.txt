cmake_minimum_required(VERSION 3.17)
project(pa0)

set(CMAKE_C_STANDARD 11)

add_executable(pa0 lab2.c)

find_package(MPI REQUIRED)
set(CMAKE_C_COMPILE_FLAGS ${CMAKE_C_COMPILE_FLAGS} ${MPI_COMPILE_FLAGS})
set(CMAKE_C_LINK_FLAGS ${CMAKE_C_LINK_FLAGS} ${MPI_LINK_FLAGS})
include_directories(${MPI_INCLUDE_PATH})
target_link_libraries(pa0 ${MPI_LIBRARIES})
