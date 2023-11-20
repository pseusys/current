.ONESHELL:
.PHONY: all build rebuild help ensure_dirs object reobject object_human reobject_human object_cube reobject_cube clean

SHELL=/bin/bash

SOURCES=main imagine objectize utils
OBJECTS=$(foreach obj,$(SOURCES),build/$(obj).o)
HEADERS=$(foreach hed,$(filter-out main,$(SOURCES)),sources/$(hed).h)

CC=gcc

CFLAGS=-g -Wall -O0 -Werror
LDFLAGS=-lm

OBJECT_NAME=frameCube

TRANSFORM_X=0.0
TRANSFORM_Y=0.0
TRANSFORM_Z=0.0
ROTATE_X=13.0
ROTATE_Y=27.0
ROTATE_Z=0.0

FOCAL=1.0
PROJECTION=pinhole
Z_PERCENT_DROP=0.0

DIMENSION_X=1024
DIMENSION_Y=1024
ASPECT_X:=$(shell bc <<< 'scale=5; 1.0/$(DIMENSION_X)')
ASPECT_Y:=$(shell bc <<< 'scale=5; 1.0/$(DIMENSION_Y)')
CENTER_X:=$(shell bc <<< 'scale=5; $(DIMENSION_X)/2.0')
CENTER_Y:=$(shell bc <<< 'scale=5; $(DIMENSION_Y)/2.0')

all: build

build: build/main.run

rebuild: clean build

help: build/main.run
	@ ./$^

ensure_dirs:
	@
	mkdir -p build
	mkdir -p processed

object: export OUTPUT_IMAGE=processed/$(OBJECT_NAME)_object_translate_$(TRANSFORM_X)_$(TRANSFORM_Y)_$(TRANSFORM_Z)_rotate_$(ROTATE_X)_$(ROTATE_Y)_$(ROTATE_Z)_dimension_$(DIMENSION_X)_$(DIMENSION_Y)_aspect_$(ASPECT_X)_$(ASPECT_Y)_center_$(CENTER_X)_$(CENTER_Y)_focal_$(FOCAL)_$(PROJECTION)_drop_$(Z_PERCENT_DROP).ppm
object: build/main.run ensure_dirs
	@
	echo "\e[33;1m**** Running app for objectivisation *****\e[0m"
	./$< objects/$(OBJECT_NAME).off $(OUTPUT_IMAGE) object $(TRANSFORM_X) $(TRANSFORM_Y) $(TRANSFORM_Z) $(ROTATE_X) $(ROTATE_Y) $(ROTATE_Z) $(DIMENSION_X) $(DIMENSION_Y) $(ASPECT_X) $(ASPECT_Y) $(CENTER_X) $(CENTER_Y) $(FOCAL) $(PROJECTION) $(Z_PERCENT_DROP)
	echo "Output objected image: $(OUTPUT_IMAGE)"
reobject: clean object

object_human:
	make --no-print-directory -s object OBJECT_NAME=human ROTATE_X=180.0 ROTATE_Y=0.0 ROTATE_Z=-90.0 TRANSFORM_Z=1.0 Z_PERCENT_DROP=0.35
reobject: clean object_human

object_cube:
	make --no-print-directory -s object OBJECT_NAME=frameCube 1>/dev/null 2>&1
reobject: clean object_cube

build/main.run: $(OBJECTS) build/Util.o ../viscomp-lab1/Util.h $(HEADERS)
	@ $(CC) -o $@ $^ $(LDFLAGS)

build/Util.o: ../viscomp-lab1/Util.c ensure_dirs
	@ $(CC) -o $@ -c $< $(CFLAGS)

build/%.o: sources/%.c ensure_dirs
	@ $(CC) -o $@ -c $< $(CFLAGS)

clean:
	@ rm -rf build processed
