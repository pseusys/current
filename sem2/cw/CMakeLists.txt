cmake_minimum_required(VERSION 3.12)
project(course_work)

set(CMAKE_CXX_STANDARD 98)
set(CMAKE_AUTOMOC ON)

include_directories(image tools window)
find_package(Qt5 REQUIRED COMPONENTS Core Widgets Gui)
add_executable(course_work image/image.cpp window/window.cpp tools/tools.cpp main.cpp)
target_link_libraries(course_work Qt5::Core Qt5::Widgets Qt5::Gui)