.ONESHELL:
.DEFAULT_GOAL := help

CONF = configs/default.ini

SHELL = /bin/bash
PATH := venv/bin:$(PATH)


controls:
	@ # Print controls info of the project
	echo "Controls:"
	echo "  - 'SPACEBAR' key"
	echo "      Restart the animation time (from 'initial_time' see README.md, configs)."
	echo "  - 'A' or 'LEFT' key"
	echo "      Move camera left by one unit."
	echo "  - 'W' or 'UP' key"
	echo "      Move camera up by one unit."
	echo "  - 'D' or 'RIGHT' key"
	echo "      Move camera right by one unit."
	echo "  - 'S' or 'DOWN' key"
	echo "      Move camera down by one unit."
	echo "  - Mouse wheel scroll"
	echo "      Zoom the simulation."
	echo "  - Mouse drag with right button pressed"
	echo "      Move camera in the direction of drag."
	echo "  - Mouse drag with left button pressed"
	echo "      Rotate camera in the direction of drag."
.PHONY: controls

help:
	@ # Print help info about the project to the STDIO
	echo "Welcome to the volcanic project for MoSIG M1 3D Graphics course!"
	echo "Greetings from the project authors: Aleksandr Sergeev, Pia Dopper and Farah Maria Majdalani :)"
	echo "The following commands are available for operating the project:"
	echo "  - make run"
	echo "      Run the project using configurations defined in the default configuration file."
	echo "      Default configuration file is named: './configs/default.ini'."
	echo "  - make run CONF=[configuration file name]"
	echo "      Run the project using configurations defined in the specified configuration file."
	echo "  - make clean"
	echo "      Clean the project virtual environment and generated python cache files."
	echo "  - make venv"
	echo "      Generate and enamble python virtual environment for the project (will be run automatically upon 'run')."
	echo "  - make controls"
	echo "      Print the runtime control info message."
	echo "  - make help"
	echo "      Print this message again."
	echo "  - make"
	echo "      Same as 'make help'."
	echo "Hope you enjoy our simulation! Cheers!"
.PHONY: help

venv:
	@ # Prepare a virtual environment for the project
	python3 -m venv venv
	pip3 install --upgrade pip
	pip3 install -r requirements.txt

run: venv controls
	@ # Run simulation using configuration file specified by CONF environmental variable
	test -n "$(CONF)" || { echo "Please, specify CONF configuration file!"; exit 1; }
	python3 -m sources.main $(CONF)
.PHONY: run

clean:
	@ # Clean all generated project files (virtual environment + python caches)
	rm -rf venv 2> /dev/null
	rm -rf **/__pycache__ 2> /dev/null
.PHONY: clean
