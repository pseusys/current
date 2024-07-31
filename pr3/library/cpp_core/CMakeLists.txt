cmake_minimum_required(VERSION 3.26)
project(FollowTheDrow)

set(CMAKE_CXX_STANDARD 17)

file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/sources/*.cpp)
list(REMOVE_ITEM SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/sources/binding.cpp)

file(GLOB HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/include/follow_the_drow/*.hpp)
list(REMOVE_ITEM HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/include/follow_the_drow/binding.hpp)

add_library(FollowTheDrow ${SOURCES})
target_include_directories(FollowTheDrow PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/follow_the_drow)

if(DEFINED ENV{ROS_ROOT})
find_package(catkin REQUIRED COMPONENTS roscpp)
target_include_directories(FollowTheDrow PRIVATE ${catkin_INCLUDE_DIRS})
target_link_libraries(FollowTheDrow PRIVATE ${catkin_LIBRARIES})
target_compile_definitions(FollowTheDrow PRIVATE -DROS_ENVIRONMENT)
endif()

install(TARGETS FollowTheDrow LIBRARY DESTINATION lib)
install(FILES ${HEADERS} DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/include)
install(FILES FollowTheDrowConfig.cmake DESTINATION cmake)
