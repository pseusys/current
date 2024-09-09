# RobAIR-TP-Sources Pyified

Based on the [sources](https://gricad-gitlab.univ-grenoble-alpes.fr/boulayen/robair-tp-sources) and [configurations](https://gricad-gitlab.univ-grenoble-alpes.fr/boulayen/robair-tp-config) repositories.  
Source files for robotics projects around [RobAIR](https://github.com/fabMSTICLig/RobAIR) as part of [Olivier Aycard's courses](https://lig-membres.imag.fr/aycard/index.php?&slt=enseignement).

Now rewritten for use with Python3 instead of C++ and moved to Docker not to be dependent on dedicated laptops.  
Project also optionally requires the [rospy for pure Python](https://github.com/rospypi/simple) library.
It might be useful in case of static analysers or linters usage.

## Build & run

Use this by running following commands:
- To run locally (`laser_text_display_node` node, `2017-09-07-16-55-12` record, defined in `example.env`):  
  ```shell
  make run ENV=example.env
  ```
- To run on actual RobAIR (`laser_text_display_node` node, `2017-09-07-16-55-12` record, defined in `example.env`):  
  ```shell
  make run NETWORK=[YOUR_ROBAIR_NETWORK_NAME] ENV=example.env
  ```
  where `YOUR_ROBAIR_NETWORK_NAME` is the name of the RobAIR network you're connected to.  
  > NB! Network names depend on your OS, use `make help` command to get a list of physical networks you are connected to.
  Still you will have to determine which of them is the RobAIR network yourself!

## Params and args

There are currently three different ways to run the project:
1. `run` (without `NETWORK` defined) - runs nodes in emulation.  
   Launches with: `roscore` (emulation root, automatically), `rosbag` (recorded simulation), `rviz` graphical user interface; using `roslaunch`.
2. `run` (with `NETWORK` defined) - runs nodes on actual RobAIR.  
   Launches with: `roslaunch` (running nodes), `rviz` graphical user interface; using `roslaunch`.

The following configuration options are supported:
1. `NETWORK` - RobAIR network name. Should be omitted when running in emulation. Has the following configuration:
   - In case of emulation: the virtual Docker network is created to avoid any possible IP collisions.  
     Container is effectively invisible outside the network and no outer networks are visible to the container as well.
   - In case of actual RobAIR: should be the RobAIR network interface name.  
     The names of the interfaces are defined by OS and thus have to be configured manually.
     If you're not sure, what is the interface name, take a look at interface discovery repost in the end of `make help` command output or consider using a tool like `ifconfig`.  
     The network IP addresses are configurable with `ROBAIR_IP` and `CONTAINER_IP` arguments.
2. `TARGET` - description of nodes to launch with `roslaunch` tool. The nodes should be described in the following way:  
   A string, formatted like: `PROJECT_1/NODE_1[:PROJECT_2/NODE_2...]`.  
   Example of such string to launch two nodes (`laser_text_display_node` and `laser_graphical_display_node`):
   ```text
   tutorial_ros/laser_text_display_node.py:tutorial_ros/laser_graphical_display_node.py
   ```
   _Example:_ `make run-node ENV=example.env TARGET=tutorial_ros/laser_text_display_node.py`
3. `LAUNCH` - description of files to include to launch with `roslaunch` tool. The files should be described in the following way:  
   A string, formatted like: `FILE_PATH_1[:FILE_PATH_2...]`.  
   Example of file to launch two nodes (`laser_text_display_node` and `laser_graphical_display_node`, file location: `./config/nodes.launch`):
   ```xml
   <launch>
       <node name="laser_text_display_node" pkg="tutorial_ros" type="laser_text_display_node.py" output="screen" />
       <node name="laser_graphical_display_node" pkg="tutorial_ros" type="laser_graphical_display_node.py" output="screen" />
   </launch>
   ```
   File format description for `roslaunch` files can be found [here](http://wiki.ros.org/roslaunch/XML).  
   _Example:_ `make run-node ENV=example.env LAUNCH=nodes.launch`
   > NB! Files should be placed into the `config` directory and file paths should be relative to it.
4. `RECORD` (required for emulation launch) - name of the file, containing recorded data to launch `rosbag` tool.  
   Records should be placed in the `assets/data` directory.  
   _Example:_ `make run ENV=example.env RECORD=2017-09-07-16-55-12.bag`
5. `ROBAIR_IP` (default value: `192.168.0.175`, used if `NETWORK` is defined only) - IP address of RobAIR to connect.  
   _Example:_ `make run-phys ENV=example.env ROBAIR_IP=192.168.0.175`
6. `CONTAINER_IP` (default value: `192.168.0.174`, used if `NETWORK` is defined only) - IP address of the container.  
   _Example:_ `make run-phys ENV=example.env CONTAINER_IP=192.168.0.174`

The following convenience configuration options are also might be handy:
1. `EXTRA=--build` might be added to any target to rebuild Docker image locally.  
   _Example:_ `make run-node ENV=example.env EXTRA=--build`
   > NB! Actually, `EXTRA` can accept any flags allowed for `docker-compose up`, but you are strictly advised to use any of them except `--build`.
   > Docker compose configuration is already done and should remain as is at least for most of the cases.
2. `SOURCES` (for `run-node` and `run-phys`, default value: `sources_py`) nodes source directory, might be set to legacy `sources_cpp` - C++ sources directory.
   Note that C++ node names do not include file extensions.  
   _Example:_ `make run-node ENV=example.env SOURCES=sources_cpp TARGET=tutorial_ros/laser_text_display_node`
3. `CONFIG` (required, default value: `config-laser.rviz`) - initial configuration file for `rviz` tool.  
   The file should be placed in the `config` directory.  
   _Example:_ `make run-node ENV=example.env CONFIG=config-laser.rviz`
4. `ENV` path to configuration file that can contain default values for all the configuration options above.
   There's a sample configuration file named `example.env`.  
   _Example:_ `make run-node ENV=example.env`
   > NB! When creating your own configuration file, consider placing it under project root and naming it with `.env` extension.
   > This way it will never end up in the container neither be version controlled.

See `make help` command output for detailed info with examples and other targets description.

## Structure

- `assets/data`  
  Data for perception
- `assets/map`  
  Map of UFR IM2AG
- `sources_*/tutorial_ros`  
  Tutorial on ROS
- `sources_*/teleoperation`  
  Lab on teleoperation
- `sources_*/follow_me`  
  Lab on "follow me" behavior: detection part
- `sources_*/localization`  
  Lab on localization

## Roadmap

1. Finish rewriting nodes in Python3.
2. Clean `CMakeLists.txt` and `package.xml` for each project.
3. Test on actual RobAIR.
4. Add `gazebo` configuration and target.

## Requirements

All of this *should* be available on every Linux + WSL:
- `Docker` version 20+
- `docker-compose` version 2+
- `make` version 3+

For Python linting and static analysis:
- `python3` version 3.6-3.8
