# Amazing generator

## Requirements

`go` version >= 1.20

## Quick run commands

1. Run with default params: `make run`
2. Clean directory: `make clean`
3. Learn more about CLI params: `make`

## Make options

1. Read maze from previously generated file: `make INPUT=[FILE_NAME]`
2. Write maze to binary file: `make OUTPUT=[FILE_NAME]`
3. Change maze size: `make SIZE=[WIDTH]x[HEIGHT]`
4. Change base image name: `make IMAGE=[BASE_IMAGE_NAME]`
5. Change image format: `make FORMAT=[ppm/svg]`
6. Enable/disable pathfinding: `make DRAW_PATH=[true/false]`
7. Change base path-file name: `make ROUTE=[BASE_IMAGE_NAME]`
8. Change maze entrance: `make ENTRANCE=[X]x[Y]`
9. Change maze exit: `make EXIT=[X]x[Y]`
10. Setup verbose output: `make VERBOSE=true`
