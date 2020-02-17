cmake_minimum_required(VERSION 3.16.4)
project(logic)

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE sources bridge/* operational/* utils/* other/* libs/* omni_header.hpp)

add_executable(LogicExec ${sources} main.cpp)

add_library(LogicDll SHARED ${sources})