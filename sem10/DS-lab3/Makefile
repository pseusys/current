.ONESHELL:
.EXPORT_ALL_VARIABLES:
.DEFAULT_GOAL := help

SHELL = /bin/bash
LOGS = log-`date +"%Y-%m-%d-%H-%M-%S"`.log
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
	python3 choreographer.py -l $(LOGS) $(CONF)
.PHONY: run

clean:
	@ # Clean generated files and locally stored libraries
	rm -rf venv >2 /dev/null
	rm -rf __pycache__ >2 /dev/null
.PHONY: clean
