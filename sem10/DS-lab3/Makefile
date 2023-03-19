.ONESHELL:
.EXPORT_ALL_VARIABLES:
.DEFAULT_GOAL := help

SHELL = /bin/bash
LOGS = logs/log-`date +"%Y-%m-%d-%H-%M-%S"`.log
VERB = INFO
PATH := venv/bin:$(PATH)


help:
	@ # Print help output
	echo ""
.PHONY: help

venv:
	@ # Create virtual python3 environment
	python3 -m venv venv
	pip3 install --upgrade pip
	pip3 install -r requirements.txt

run: venv
	@ # Run selected configuration
	test -n "$(CONF)" || { echo "Please, specify CONF configuration file!"; exit 1; }
	python3 sources/choreographer.py -l $(LOGS) -v $(VERB) $(CONF)
.PHONY: run

clean:
	@ # Clean generated files and locally stored libraries
	rm -rf venv 2> /dev/null
.PHONY: clean

rabbit:
	docker run -it --rm --name Rabbitmq -p 5672:5672 -p 15672:15672 rabbitmq:3.11-management
.PHONY: rabbit
