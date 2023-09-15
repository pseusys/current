#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"
#include "maze.h"
#include "svg.h"
#include "ppm.h"

/* Save maze using desired format and generated filename */
void save_maze (maze* m, char* w, char* h, char* f) {
  char* template = "maze-%sx%s.%s";
  char* filename = calloc(strlen(template) - 5 + strlen(w) + strlen(h) + strlen(f), sizeof(char));
  sprintf(filename, template, w, h, f);

  if (strcmp(f, "svg") == 0) maze_svg(m, filename);
  else if (strcmp(f, "ppm") == 0) maze_ppm(m, filename);

  printf("Output image written: '%s'\n", filename);
  free(filename);
}

int main (int argc, char** argv) {
  if (argc != 4) {
    printf("Usage: %s <width> <height> <format>\n", argv[0]);
    return EXIT_FAILURE;
  }

  int width = atoi(argv[1]), height = atoi(argv[2]);
  if (width <= 0 || height <= 0) {
    printf("Both width and height of the maze should be >= 0!\n");
    return EXIT_FAILURE;
  }

  if (strcmp(argv[3], "svg") != 0 && strcmp(argv[3], "ppm") != 0) {
    printf("Format must be one of 'svg' or 'ppm'!\n");
    return EXIT_FAILURE;
  }

  maze *m = maze_random(width, height);
  save_maze(m, argv[1], argv[2], argv[3]);
  maze_free(m);

  return EXIT_SUCCESS;
}
