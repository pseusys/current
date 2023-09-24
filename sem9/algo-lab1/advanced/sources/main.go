package main

import (
	"container/list"
	"errors"
	"flag"
	"fmt"
	"os"
)

const (
	NONE_STRING      = ""
	DEF_IMAGE_NAME   = "maze"
	DEF_ROUTE_NAME   = "path"
	DEF_SIZE         = "512x512"
	DEF_VERBOSE      = false
	DEF_PATH         = true
	VECTOR_FORMAT    = "svg"
	BINARY_FORMAT    = "ppm"
	FILE_PERMISSIONS = 0644
)

var (
	input    = flag.String("i", NONE_STRING, "Input maze file (default: will be generated)")
	output   = flag.String("o", NONE_STRING, "Output maze file (default: no file output)")
	image    = flag.String("n", DEF_IMAGE_NAME, fmt.Sprintf("Output image base name (default: '%s')", DEF_IMAGE_NAME))
	format   = flag.String("f", BINARY_FORMAT, fmt.Sprintf("Output image format, should be either 'ppm' or 'svg' (default: '%s')", BINARY_FORMAT))
	size     = flag.String("s", DEF_SIZE, fmt.Sprintf("Size of maze, formatted like [WIDTH]x[HEIGHT] (default: '%s')", DEF_SIZE))
	route    = flag.String("r", DEF_ROUTE_NAME, fmt.Sprintf("Output path base name (default: '%s')", DEF_ROUTE_NAME))
	entrance = flag.String("b", NONE_STRING, "Maze entrance, formatted like [X]x[Y] (defualt: will be generated)")
	exit     = flag.String("e", NONE_STRING, "Maze exit, formatted like [X]x[Y] (defualt: will be generated)")
	verbose  = flag.Bool("v", DEF_VERBOSE, fmt.Sprintf("Print intermediate output (default: %v)", DEF_VERBOSE))
	path     = flag.Bool("p", DEF_PATH, fmt.Sprintf("Calculate and draw path from entrance to exit (default: %v)", DEF_PATH))
	help     = flag.Bool("h", false, "Print this message again and exit")
)

var (
	MAZE_WIDTH      uint
	MAZE_HEIGHT     uint
	MAZE_ENTRANCE_X uint
	MAZE_ENTRANCE_Y uint
	MAZE_EXIT_X     uint
	MAZE_EXIT_Y     uint
)

func main() {
	var err error
	flag.Parse()
	if *help {
		flag.Usage()
		return
	}

	var maze *Maze
	if *input != NONE_STRING {
		LogF("Reading maze from file '%s'\n", *input)
		data, err := os.ReadFile(*input)
		if err != nil {
			LogE("error reading from file", err)
		}
		maze, err = ParseMaze(data)
		if err != nil {
			LogE("error parsing maze file", err)
		}
		MAZE_WIDTH = maze.tree.width
		MAZE_HEIGHT = maze.tree.height
	} else {
		fmt.Sscanf(*size, "%dx%d", &MAZE_WIDTH, &MAZE_HEIGHT)
		LogF("Generating maze of size: %dx%d\n", MAZE_WIDTH, MAZE_HEIGHT)
		maze, err = GenerateMaze(MAZE_WIDTH, MAZE_HEIGHT)
		if err != nil {
			LogE("error generating maze", err)
		} else if MAZE_WIDTH == 1 || MAZE_HEIGHT == 1 {
			LogE("maze of size 1", errors.New("it's boring"))
		}
	}

	var path_list *list.List
	if *path {
		if *entrance != NONE_STRING {
			fmt.Sscanf(*entrance, "%dx%d", &MAZE_ENTRANCE_X, &MAZE_ENTRANCE_Y)
		} else {
			MAZE_ENTRANCE_X = RandUint() % (MAZE_WIDTH - 1)
			MAZE_ENTRANCE_Y = RandUint() % (MAZE_HEIGHT - 1)
		}
		if *exit != NONE_STRING {
			fmt.Sscanf(*exit, "%dx%d", &MAZE_EXIT_X, &MAZE_EXIT_Y)
		} else {
			MAZE_EXIT_X = (RandUint() % (MAZE_WIDTH - MAZE_ENTRANCE_X)) + MAZE_ENTRANCE_X
			MAZE_EXIT_Y = (RandUint() % (MAZE_HEIGHT - MAZE_ENTRANCE_Y)) + MAZE_ENTRANCE_Y
		}
		LogF("Calculating path from %dx%d to %dx%d\n", MAZE_ENTRANCE_X, MAZE_ENTRANCE_Y, MAZE_EXIT_X, MAZE_EXIT_Y)

		path_list, err = maze.BuildPath(MAZE_ENTRANCE_X, MAZE_ENTRANCE_Y, MAZE_EXIT_X, MAZE_EXIT_Y)
		if err != nil {
			LogE("error building path", err)
		}

		path_file := fmt.Sprintf("%s-%dx%d-%dx%d.txt", *route, MAZE_ENTRANCE_X, MAZE_ENTRANCE_Y, MAZE_EXIT_X, MAZE_EXIT_Y)
		os.WriteFile(path_file, []byte(maze.SprintPath(path_list, false, true)), FILE_PERMISSIONS)
	}

	maze.SaveSVG(fmt.Sprintf("%s-%dx%d.%s", *image, MAZE_WIDTH, MAZE_HEIGHT, *format), path_list)

	if *output != NONE_STRING {
		data, err := maze.DumpMaze()
		if err != nil {
			LogE("error dumping maze", err)
		}
		os.WriteFile(*output, data, FILE_PERMISSIONS)
	}
}
