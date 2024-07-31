cmake_minimum_required(VERSION 3.26)
project(follow_the_drow)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE RelWithDebInfo)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  rospy
  tf
  std_msgs
  sensor_msgs
  nav_msgs
  message_generation
  visualization_msgs
  geometry_msgs
  genmsg
)

add_message_files(FILES
  raw_data.msg
  detection.msg
)

catkin_python_setup()

generate_messages(DEPENDENCIES
  geometry_msgs
)

catkin_package(INCLUDE_DIRS CATKIN_DEPENDS
  roscpp
  rospy
  tf
  std_msgs
  sensor_msgs
  nav_msgs
  message_generation
  message_runtime
  visualization_msgs
  geometry_msgs
  genmsg
)

find_package(FollowTheDrow CONFIG REQUIRED)

include_directories(
  include
  ${catkin_INCLUDE_DIRS}
  $ENV{FollowTheDrow_INCLUDE_DIRS}
)

link_directories(
  ${catkin_LIBRARY_DIRS}
  $ENV{FollowTheDrow_LIBRARY_DIRS}
)

set(UTIL_SOURCES src/main.cpp src/utils.cpp src/colors.cpp src/tracker_policy.cpp)

add_executable(live_loader src/live_loader.cpp ${UTIL_SOURCES})
add_dependencies(live_loader ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
target_link_libraries(live_loader ${catkin_LIBRARIES} FollowTheDrow)

add_executable(visualizer src/visualizer.cpp ${UTIL_SOURCES})
add_dependencies(visualizer ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
target_link_libraries(visualizer ${catkin_LIBRARIES} FollowTheDrow)

add_executable(algorithmic_detector src/algorithmic_detector.cpp ${UTIL_SOURCES})
add_dependencies(algorithmic_detector ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
target_link_libraries(algorithmic_detector ${catkin_LIBRARIES} FollowTheDrow)

add_executable(person_tracker src/person_tracker.cpp ${UTIL_SOURCES})
add_dependencies(person_tracker ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
target_link_libraries(person_tracker ${catkin_LIBRARIES} FollowTheDrow)

catkin_install_python(PROGRAMS
  nodes/file_loader.py
  nodes/drow_detector.py
  nodes/data_annotator.py
  DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)
