.ONESHELL:
.PHONY: all build rebuild help ensure_dirs test filter refilter histogram rehistogram stretch restretch equalize reequalize clean

SOURCES=main gaussian average mean histogram imagine alter padding
OBJECTS=$(foreach obj,$(SOURCES),build/$(obj).o)
HEADERS=$(foreach hed,$(filter-out main,$(SOURCES)),sources/$(hed).h)

CC=gcc

CFLAGS=-g -Wall -O0 -Werror
LDFLAGS=

FILTER_TIMES=1
FILTER_DIMENSION=11
FILTER_TYPE=average
FILTER_PADDING=mean

IMAGE_NAME=dog

all: build

build: build/main.run

rebuild: clean build

help: build/main.run
	@ ./$^

ensure_dirs:
	@
	mkdir -p build
	mkdir -p processed

test_filter: build/main.run ensure_dirs
	@
	echo "\e[33;1m**** Testing app for $(FILTER_TYPE) filters from 3 to 13 *****\e[0m"
	$(foreach var,$(shell seq 3 2 13),./$< pictures/$(IMAGE_NAME).pgm processed/$(IMAGE_NAME)_filtered_$(FILTER_TYPE)_$(var)_padding_$(FILTER_PADDING).pgm filter $(FILTER_TYPE) $(FILTER_PADDING) 1 $(var);)

filter: export OUTPUT_IMAGE=processed/$(IMAGE_NAME)_filtered_$(FILTER_TYPE)_$(FILTER_DIMENSION)_padding_$(FILTER_PADDING).pgm
filter: build/main.run ensure_dirs
	@
	echo "\e[33;1m**** Running app for $(FILTER_TYPE) filter of size $(FILTER_DIMENSION) *****\e[0m"
	./$< pictures/$(IMAGE_NAME).pgm $(OUTPUT_IMAGE) filter $(FILTER_TYPE) $(FILTER_PADDING) $(FILTER_TIMES) $(FILTER_DIMENSION)
	echo "Output image: $(OUTPUT_IMAGE)"

refilter: clean filter

histogram: export OUTPUT_IMAGE=processed/$(IMAGE_NAME)_histogram.svg
histogram: build/main.run ensure_dirs
	@
	echo "\e[33;1m**** Running app for histogram *****\e[0m"
	./$< pictures/$(IMAGE_NAME).pgm $(OUTPUT_IMAGE) histogram
	echo "Output histogram: $(OUTPUT_IMAGE)"

rehistogram: clean histogram

stretch: export OUTPUT_IMAGE=processed/$(IMAGE_NAME)_stretched.pgm
stretch: build/main.run ensure_dirs
	@
	echo "\e[33;1m**** Running app for stretching *****\e[0m"
	./$< pictures/$(IMAGE_NAME).pgm $(OUTPUT_IMAGE) stretch
	echo "Output histogram: $(OUTPUT_IMAGE)"

restretch: clean stretch

equalize: export OUTPUT_IMAGE=processed/$(IMAGE_NAME)_equalized.pgm
equalize: build/main.run ensure_dirs
	@
	echo "\e[33;1m**** Running app for equalization *****\e[0m"
	./$< pictures/$(IMAGE_NAME).pgm $(OUTPUT_IMAGE) equalize
	echo "Output histogram: $(OUTPUT_IMAGE)"

reequalize: clean stretch

build/main.run: $(OBJECTS) build/Util.o ../viscomp-lab1/Util.h $(HEADERS)
	@ $(CC) -o $@ $^ $(LDFLAGS) -lm

build/Util.o: ../viscomp-lab1/Util.c ensure_dirs
	@ $(CC) -o $@ -c $< $(CFLAGS)

build/%.o: sources/%.c ensure_dirs
	@ $(CC) -o $@ -c $< $(CFLAGS) -lm

clean:
	@ rm -rf build processed
