.ONESHELL:
.PHONY: all build rebuild help ensure_dirs object reobject clean

SOURCES=main imagine objectize utils
OBJECTS=$(foreach obj,$(SOURCES),build/$(obj).o)
HEADERS=$(foreach hed,$(filter-out main,$(SOURCES)),sources/$(hed).h)

CC=gcc

CFLAGS=-g -Wall -O0 -Werror
LDFLAGS=-lm


OBJECT_NAME=human

all: build

build: build/main.run

rebuild: clean build

help: build/main.run
	@ ./$^

ensure_dirs:
	@
	mkdir -p build
	mkdir -p processed

object: export OUTPUT_IMAGE=processed/$(OBJECT_NAME)_object.ppm
object: build/main.run ensure_dirs
	@
	echo "\e[33;1m**** Running app for obhectivisation *****\e[0m"
	./$< objects/$(OBJECT_NAME).off $(OUTPUT_IMAGE) object
	echo "Output objected image: $(OUTPUT_IMAGE)"
reobject: clean object

build/main.run: $(OBJECTS) build/Util.o ../viscomp-lab1/Util.h $(HEADERS)
	@ $(CC) -o $@ $^ $(LDFLAGS)

build/Util.o: ../viscomp-lab1/Util.c ensure_dirs
	@ $(CC) -o $@ -c $< $(CFLAGS)

build/%.o: sources/%.c ensure_dirs
	@ $(CC) -o $@ -c $< $(CFLAGS)

clean:
	@ rm -rf build processed
