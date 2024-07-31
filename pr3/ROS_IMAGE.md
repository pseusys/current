# ROS desktop Docker image

Among other things and scripts, this repo contains a docker image and configuration for running ROS `noetic` in full configuration in Docker.

## Motivation

The latest ROS 1 distribution ([ROS 1 Noetic Ninjemys](http://wiki.ros.org/noetic/Installation)) is targeted for **Ubuntu Focal Fossa (20.04)**.
Its installation on later Ubuntu (or other Linux distributions) is difficult, not officially supported and can result in unknown amount of bugs.
Provided Docker image is aimed to solve the problem.

## Image

The image configuration can be found in `deploy/docker` directory.
You will be interested in `basic` target in Dockerfile; the rest is about `follow_the_drow` library, that is not universally needed.
The image includes:

- [`GDB`](https://www.sourceware.org/gdb/) for C++ based node debugging.
- Python 3.8 alongside with `python3-dev` (for Python to C++ interop) `python3-pip` (for external package installation).
- ROS [`desktop-full`](https://packages.ubuntu.com/jammy/ros-desktop-full) distribution (find out what it includes [here](https://www.ros.org/reps/rep-0003.html#noetic-ninjemys-may-2020-may-2025)).
- Configured `catkin` environment under `/~/catkin_ws` root (set as workdir).

## Build and Pull

The image is available in GitHub image registry under name `ros_complete_env:main` and can be pulled from there.
Use the following command to do that:

```bash
docker pull ghcr.io/pseusys/follow_the_drow/ros_complete_env:main
```

It can be re-built using make `build-image` target, defined in `Makefile`:

```bash
make build-image
```

## Run your Nodes

In order to run your own nodes, take a look at `deploy/docker/docker-compose.yml` file.
In this file the environment and network configuration for running ROS in container is provided.
Please, note the files, directories and environmental variables it depends on:

1. `deploy/out` volume: the place for all **generated** files (e.g. ROS bags).
2. `deploy/config` volume: the place for all **configuration** files (e.g. [`launch`](http://wiki.ros.org/roslaunch/XML) files, RVIZ configs, etc.).
3. `deploy/follow_the_drow` volume: will be placed directly into `catkin_ws/src` - will be treated as ROS package.
4. `deploy/conf.env` file: environment variables that will be available in container.
5. `ROBAIR_IP` environment variable: should match RobAIR physical IP or be `127.0.0.1` for running nodes locally.
6. `CONTAINER_IP` environment variable:  should be equal to the host IP in robair network or be `127.0.0.1` for running nodes locally.
7. `DISPLAY` environment variable: should be set by default, sets the display for ROS windows to appear on.

> In order to run your own packages, include them the same way as `deploy/follow_the_drow` volume is included and set up environment and configurations for them in corresponding volumes.

For running the nodes, take a look at make `launch-docker-*` targets, defined in `Makefile`.

> Note: the command you would like to launch in the container should be provided as `command` argument (see `deploy/docker/docker-compose.yml` file).
> There is no need to source anything or run `catkin_make`, it will be run automatically; take a look at `deploy/docker/entrypoint.sh` to see the exact prefix for each command.

Running nodes locally:

```bash
make launch-docker-local
```

Running nodes on RobAIR:

```bash
make launch-docker-robot
```

> In **my** case the IP of the RobAIR I was working with was `192.168.1.201`. In your case it might be different, so change accordingly.
