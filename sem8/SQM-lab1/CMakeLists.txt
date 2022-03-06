cmake_minimum_required(VERSION 3.17)
project(SQM_lab1 C)

set(CMAKE_C_STANDARD 11)

add_executable(SQM_lab1 main.c)
target_link_libraries(SQM_lab1 m)
