EXECUTABLE_NAME = maze.run

INPUT=
OUTPUT=maze.bin
IMAGE=maze
FORMAT=ppm
SIZE=64x64
DRAW_PATH=true
ROUTE=path
ENTRANCE=0x0
EXIT=63x63
VERBOSE=false

all: help

build:
	go mod tidy
	go build -o build/$(EXECUTABLE_NAME) ./sources

rebuild: clean build
.PHONY: rebuild

help: build
	build/$(EXECUTABLE_NAME) -h
.PHONY: help

run: build
ifdef $(INPUT)
	build/$(EXECUTABLE_NAME) -i $(INPUT) -o $(OUTPUT) -n $(IMAGE) -f $(FORMAT) -p $(DRAW_PATH) -r $(ROUTE) -b $(ENTRANCE) -e $(EXIT) -v $(VERBOSE)
else
	build/$(EXECUTABLE_NAME) -o $(OUTPUT) -n $(IMAGE) -f $(FORMAT) -s $(SIZE) -p $(DRAW_PATH) -r $(ROUTE) -b $(ENTRANCE) -e $(EXIT) -v $(VERBOSE)
endif
.PHONY: run

rerun: clean run
.PHONY: rerun

lint:
	golint .
	go fmt ./sources
.PHONY: lint

clean:
	rm -rf build
	rm -f go.sum *.bin *.ppm *.svg *.txt
.PHONY: clean
