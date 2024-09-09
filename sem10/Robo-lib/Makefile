.ONESHELL:
.EXPORT_ALL_VARIABLES:
.DEFAULT_GOAL := help

SHELL = /bin/bash
PATH := venv/bin:$(PATH)


CONFIG = config-laser.rviz
SOURCES = sources_py
EXTRA =

ROBAIR_IP =
CONTAINER_IP =

NETWORK =
LAUNCH =
TARGET =
RECORD =

ENV = example.env
include $(ENV)


help:
	@ # Print help output
	echo "Welcome to 'robair-pyified'!"
	echo "The following commands are available:"
	echo "- 'make run':"
	echo "	Run node in emulation or on actual RobAIR if network is defined."
	echo "- 'make clean':"
	echo "	Clean all locally built docker images and remove containers and venv directory."
	echo "- 'make venv':"
	echo "	Install 'roslibpy' python package locally for linting, IDE static analysis and testing purposes."
	echo "- 'make help' (equal to 'make'):"
	echo "	Display this message again."
	echo -e "\nBonus content:"
	echo "- Add 'EXTRA=--build' argument to any command to build 'robair-pyified' image locally instead of pulling."
	echo "- Add 'SOURCES=sources_cpp' argument to 'run' command to use legacy C++ nodes instead of default Python."
	echo "- Any of the arguments can be read from .ENV file using 'ENV=<ENV_FILE_NAME>' parameter."
	echo "See the argument description in the 'README.md' file!"
	echo -e "\nNetwork interface discovery report can be found below:"
	ls -l /sys/class/net/ | grep -v virtual | awk '{print $$9}' | grep '\S'| while read -r interface; do
		echo -n "- '$$interface': "
		[[ -d  /sys/class/net/"$$interface"/wireless ]] && echo -n "WIRELESS " || echo -n "WIRED "
		[[ `cat /sys/class/net/"$$interface"/operstate` == "up" ]] && echo -n "UP " || echo -n "DOWN "
		[[ `cat /sys/class/net/"$$interface"/carrier` == "1" ]] && echo "ONLINE" || echo "OFFLINE"
	done
.PHONY: help


run:
	@ # Run TARGET in simulation if NETWORK is not defined or on actual RobAIR otherwise
ifdef NETWORK
	$(MAKE) run-phys LAUNCH=ros-phys.launch:$(LAUNCH) ROBAIR_IP=192.168.0.175 CONTAINER_IP=192.168.0.174
else
	$(MAKE) run-node LAUNCH=ros-node.launch:$(LAUNCH) ROBAIR_IP=127.0.0.1 CONTAINER_IP=127.0.0.1
endif
.PHONY: run

run-node:
	@ # Run target TARGET on record RECORD in simulation
	test -n "$(RECORD)" || { echo "Please, specify RECORD env var!"; exit 1; }
	xhost +local:docker
	docker-compose -f ./docker/docker-compose-node.yml up --force-recreate --attach roslaunch $(EXTRA)
.PHONY: run-node

run-phys:
	@ # Run target TARGET on actual RobAIR with ROBAIR_IP
	test -n "$(NETWORK)" || { echo "Please, specify NETWORK env var!"; exit 1; }
	xhost +local:docker
	docker-compose -f ./docker/docker-compose-phys.yml up --force-recreate --attach roslaunch $(EXTRA)
.PHONY: run-phys


venv:
	@ # Create virtual python environment for linting purposes
	python3 -m venv venv
	pip3 install --upgrade pip
	pip3 install --extra-index-url https://rospypi.github.io/simple/ rospy
	pip3 install --extra-index-url https://rospypi.github.io/simple/ tf2_ros

clean:
	@ # Remove created docker containers and venv dir
	docker rm -f roslaunch-combined-environment 2> /dev/null || true
	docker rmi ghcr.io/pseusys/robair-pyified/ros-desktop-settled:master 2> /dev/null || true
	rm -rf venv 2> /dev/null
.PHONY: clean
