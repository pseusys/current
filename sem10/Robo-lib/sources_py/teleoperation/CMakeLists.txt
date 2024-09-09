cmake_minimum_required(VERSION 2.8.2)
project(teleoperation)

catkin_package(
    CATKIN_DEPENDS geometry_msgs
)

include_directories(
    include ${catkin_INCLUDE_DIRS}
)

catkin_install_python(PROGRAMS
    src/teleoperation_node.py
	DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)
