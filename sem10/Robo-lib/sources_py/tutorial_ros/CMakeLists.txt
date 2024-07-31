cmake_minimum_required(VERSION 2.8.3)
project(tutorial_ros)

catkin_package(
    CATKIN_DEPENDS std_msgs sensor_msgs geometry_msgs visualization_msgs
)

include_directories(
    include ${catkin_INCLUDE_DIRS}
)

catkin_install_python(PROGRAMS
    src/laser_text_display_node.py
    src/laser_graphical_display_node.py
    DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)
