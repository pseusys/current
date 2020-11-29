cmake_minimum_required(VERSION 3.17)
project(PA_lab5 C)

set(CMAKE_C_STANDARD 11)

add_executable(PA_lab5 main10.c main6.c main7.c)

find_package(MPI REQUIRED)
set(CMAKE_C_COMPILE_FLAGS ${CMAKE_C_COMPILE_FLAGS} ${MPI_COMPILE_FLAGS})
set(CMAKE_C_LINK_FLAGS ${CMAKE_C_LINK_FLAGS} ${MPI_LINK_FLAGS})
include_directories(${MPI_INCLUDE_PATH})
target_link_libraries(PA_lab5 ${MPI_LIBRARIES})