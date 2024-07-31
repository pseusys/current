FROM ros:noetic-ros-base-focal AS basic

SHELL ["/bin/bash", "-c"]

ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install --no-install-recommends -y gdb python3-dev python3-pip ros-noetic-desktop-full && rm -rf /var/lib/apt/lists/*
ENV PYTHONUNBUFFERED 1
ENV PYTHONDONTWRITEBYTECODE 1

ENV ROS_LANG_DISABLE geneus:genlisp:gennodejs
RUN mkdir -p /~/catkin_ws/src && cd /~/catkin_ws && source /opt/ros/noetic/setup.bash && catkin_make clean && catkin_make && catkin_make install
WORKDIR /~/catkin_ws

COPY deploy/docker/entrypoint.sh ./
ENTRYPOINT ["bash", "entrypoint.sh"]
CMD ["rosversion", "ros"]


FROM basic as ftd

COPY library /~
RUN pip install /~ && find /~ -maxdepth 1 -mindepth 1 ! \( -name 'cpp_core' -o -name 'entrypoint.sh' -o -name 'catkin_ws' \) -exec rm -rf {} + && mv /~/cpp_core /~/fillow_the_drow_library

ADD https://cmake.org/files/v3.26/cmake-3.26.1-linux-x86_64.sh /tmp/cmake-install.sh
RUN mkdir /opt/cmake-3.26.1 && bash /tmp/cmake-install.sh --skip-license --prefix=/opt/cmake-3.26.1 && rm /tmp/cmake-install.sh && ln -s /opt/cmake-3.26.1/bin/* /usr/local/bin

ENV CMAKE_INSTALL_PREFIX /usr/local
RUN mkdir /~/fillow_the_drow_library/build && cd /~/fillow_the_drow_library/build && source /opt/ros/noetic/setup.bash && cmake .. && make && make install
ENV FollowTheDrow_INCLUDE_DIRS /~/fillow_the_drow_library/include
ENV FollowTheDrow_LIBRARY_DIRS ${CMAKE_INSTALL_PREFIX}/lib
ENV FollowTheDrow_DIR ${CMAKE_INSTALL_PREFIX}/cmake
