.ONESHELL:
.PHONY: all build rebuild help ensure_dirs filter refilter histogram rehistogram stretch restretch equalize reequalize gradient regradient nonmax renonmax hysteresis rehysteresis segment resegment clean

SOURCES=main filter histogram imagine alter padding gradient cannys segmentation
OBJECTS=$(foreach obj,$(SOURCES),build/$(obj).o)
HEADERS=$(foreach hed,$(filter-out main,$(SOURCES)),sources/$(hed).h)

CC=gcc

CFLAGS=-g -Wall -O0 -Werror
LDFLAGS=-lm

FILTER_TIMES=1
FILTER_DIMENSION=11
FILTER_TYPE=average
FILTER_PADDING=mean

GRADIENT_TYPE=scharr
GRADIENT_THRESHOLD=100

HYSTERESIS_LOWER=32
HYSTERESIS_HIGHER=128

SEGMENT_CORDINALITY=rgb-location
SEGMENT_CLUSTERS=3

IMAGE_NAME=frog

all: build

build: build/main.run

rebuild: clean build

help: build/main.run
	@ ./$^

ensure_dirs:
	@
	mkdir -p build
	mkdir -p processed

filter: export OUTPUT_IMAGE=processed/$(IMAGE_NAME)_filtered_$(FILTER_TYPE)_$(FILTER_DIMENSION)_padding_$(FILTER_PADDING)_times_$(FILTER_TIMES).ppm
filter: build/main.run ensure_dirs
	@
	echo "\e[33;1m**** Running app for $(FILTER_TYPE) filter of size $(FILTER_DIMENSION) *****\e[0m"
	./$< pictures/$(IMAGE_NAME).ppm $(OUTPUT_IMAGE) filter $(FILTER_TYPE) $(FILTER_PADDING) $(FILTER_TIMES) $(FILTER_DIMENSION)
	echo "Output image: $(OUTPUT_IMAGE)"

refilter: clean filter

histogram: export OUTPUT_IMAGE=processed/$(IMAGE_NAME)_histogram.svg
histogram: build/main.run ensure_dirs
	@
	echo "\e[33;1m**** Running app for histogram *****\e[0m"
	./$< pictures/$(IMAGE_NAME).ppm $(OUTPUT_IMAGE) histogram
	echo "Output histogram: $(OUTPUT_IMAGE)"

rehistogram: clean histogram

stretch: export OUTPUT_IMAGE=processed/$(IMAGE_NAME)_stretched.ppm
stretch: build/main.run ensure_dirs
	@
	echo "\e[33;1m**** Running app for stretching *****\e[0m"
	./$< pictures/$(IMAGE_NAME).ppm $(OUTPUT_IMAGE) stretch
	echo "Output histogram: $(OUTPUT_IMAGE)"

restretch: clean stretch

equalize: export OUTPUT_IMAGE=processed/$(IMAGE_NAME)_equalized.ppm
equalize: build/main.run ensure_dirs
	@
	echo "\e[33;1m**** Running app for equalization *****\e[0m"
	./$< pictures/$(IMAGE_NAME).ppm $(OUTPUT_IMAGE) equalize
	echo "Output histogram: $(OUTPUT_IMAGE)"

reequalize: clean stretch

gradient: export OUTPUT_IMAGE=processed/$(IMAGE_NAME)_gradient_$(GRADIENT_TYPE)_$(GRADIENT_THRESHOLD).ppm
gradient: build/main.run ensure_dirs
	@
	echo "\e[33;1m**** Running app for gradient calculation *****\e[0m"
	./$< pictures/$(IMAGE_NAME).ppm $(OUTPUT_IMAGE) gradient $(GRADIENT_TYPE) $(FILTER_PADDING) $(GRADIENT_THRESHOLD)
	echo "Output gradient: $(OUTPUT_IMAGE)"
regradient: clean gradient

nonmax: export OUTPUT_IMAGE=processed/$(IMAGE_NAME)_nonmax_$(GRADIENT_TYPE).ppm
nonmax: build/main.run ensure_dirs
	@
	echo "\e[33;1m**** Running app for nonmax suppression calculation *****\e[0m"
	./$< pictures/$(IMAGE_NAME).ppm $(OUTPUT_IMAGE) nonmax $(GRADIENT_TYPE) $(FILTER_PADDING)
	echo "Output nonmax suppressed image: $(OUTPUT_IMAGE)"
renonmax: clean nonmax

hysteresis: export OUTPUT_IMAGE=processed/$(IMAGE_NAME)_hysteresis_$(GRADIENT_TYPE)_$(HYSTERESIS_HIGHER)_$(HYSTERESIS_LOWER).ppm
hysteresis: build/main.run ensure_dirs
	@
	echo "\e[33;1m**** Running app for hysteresis thresholding *****\e[0m"
	./$< pictures/$(IMAGE_NAME).ppm $(OUTPUT_IMAGE) hysteresis $(GRADIENT_TYPE) $(FILTER_PADDING) $(HYSTERESIS_HIGHER) $(HYSTERESIS_LOWER)
	echo "Output hysteresis thresholded image: $(OUTPUT_IMAGE)"
rehysteresis: clean hysteresis

segment: export OUTPUT_IMAGE=processed/$(IMAGE_NAME)_segment_$(SEGMENT_CORDINALITY)_$(SEGMENT_CLUSTERS).ppm
segment: build/main.run ensure_dirs
	@
	echo "\e[33;1m**** Running app for segmentation *****\e[0m"
	./$< pictures/$(IMAGE_NAME).ppm $(OUTPUT_IMAGE) segment $(SEGMENT_CORDINALITY) $(SEGMENT_CLUSTERS)
	echo "Output segmented image: $(OUTPUT_IMAGE)"
resegment: clean segment

build/main.run: $(OBJECTS) build/Util.o ../viscomp-lab1/Util.h $(HEADERS)
	@ $(CC) -o $@ $^ $(LDFLAGS)

build/Util.o: ../viscomp-lab1/Util.c ensure_dirs
	@ $(CC) -o $@ -c $< $(CFLAGS)

build/%.o: sources/%.c ensure_dirs
	@ $(CC) -o $@ -c $< $(CFLAGS)

clean:
	@ rm -rf build processed
