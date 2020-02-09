cmake_minimum_required(VERSION 3.12)
project(untitled1)

set(CMAKE_CXX_STANDARD 17)

add_executable(untitled1 EquationSet.cpp EquationSet.h main.cpp)

add_library(OOP-lab0 SHARED EquationSet.cpp EquationSet.h main.cpp)