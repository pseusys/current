.ONESHELL:

SHELL = /bin/bash
PATH := venv/bin:$(PATH)


venv:
	@ # Prepare a virtual environment for the project
	python3 -m venv venv
	pip3 install --upgrade pip
	pip3 install numpy torch pandas matplotlib seaborn scikit-learn lightning jupyter
	jupyter nbextension enable --py widgetsnbextension
