cmake_minimum_required(VERSION 3.17)
project(PA_lab3 C)

set(CMAKE_C_STANDARD 11)

add_executable(PA_lab3 main.c)

find_package(MPI REQUIRED)
set(CMAKE_C_COMPILE_FLAGS ${CMAKE_C_COMPILE_FLAGS} ${MPI_COMPILE_FLAGS})
set(CMAKE_C_LINK_FLAGS ${CMAKE_C_LINK_FLAGS} ${MPI_LINK_FLAGS})
include_directories(${MPI_INCLUDE_PATH})
target_link_libraries(PA_lab3 ${MPI_LIBRARIES})