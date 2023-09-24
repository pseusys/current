EXECUTABLE_NAME = maze.run

all: run

build:
	go mod tidy
	go build -o build/$(EXECUTABLE_NAME) ./sources
.PHONY: build

run: build
	build/$(EXECUTABLE_NAME) -v -s 10x10
.PHONY: run

lint:
	golint .
	go fmt ./sources
.PHONY: lint

clean:
	rm -rf build
	rm -f go.sum
.PHONY: clean
