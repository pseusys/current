registry:
	(cd build && rmiregistry)
.PHONY: registry

compile_server:
	javac -d build -sourcepath src src/HelloServer.java
.PHONY: compile_server

compile_client:
	javac -d build -sourcepath src src/HelloClient.java
.PHONY: compile_client

server: compile_server
	java -cp build HelloServer
.PHONY: server

client: compile_client
	java -cp build HelloClient
.PHONY: client
