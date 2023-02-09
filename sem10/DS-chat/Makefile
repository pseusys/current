.ONESHELL:
.EXPORT_ALL_VARIABLES:
.DEFAULT_GOAL := help

SHELL = /bin/bash
NAME = $$(echo $$RANDOM | md5sum | head -c 5)


.help:
	@
	echo "help"
.PHONY: help

server:
	javac -d build -sourcepath src src/ChatServer.java
	java -cp build ChatServer
.PHONY: server

client:
	javac -d build -sourcepath src src/ChatClient.java
	java -cp build ChatClient -n "$(NAME)" -h
.PHONY: client
