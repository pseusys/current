#include <stdlib.h>
#include <stdbool.h>
#include "ppm.h"

/* Image width and height */
unsigned int ppm_width, ppm_height;

/* Maximum intensity value */
unsigned int ppm_intensity = 255;

/* PPM drawing colors */
unsigned char ppm_back_color[3] = {220, 220, 220};
unsigned char ppm_wall_color[3] = {56, 56, 56};
unsigned char ppm_door_color[3] = {222, 184, 135};

/* Fills picture buffer with specified color */
void fill_buffer (unsigned char* color, unsigned char* buffer, unsigned int size) {
  for (int i = 0; i < size; i++) for (int j = 0; j < 3; j++) buffer[i * 3 + j] = color[j];
}

/* Draws wall according to coordinate grid */
void set_wall (unsigned char* color, unsigned char* buffer, unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, bool is_door) {
  int line_start, line_end, line_coord;
  if (x1 == x2) {

    line_coord = x1 * 4 + 3;
    if (y1 > y2) {
      line_start = y2 * 4 + 3 + is_door;
      line_end = y1 * 4 + 3 - is_door;
    } else {
      line_start = y1 * 4 + 3 + is_door;
      line_end = y2 * 4 + 3 - is_door;
    }

    for (int i = line_start; i <= line_end; i++)
      for (int j = 0; j < 3; j++)
        buffer[(i * ppm_width + line_coord) * 3 + j] = color[j];

  } else if (y1 == y2) {
    
    line_coord = y1 * 4 + 3;
    if (x1 > x2) {
      line_start = x2 * 4 + 3 + is_door;
      line_end = x1 * 4 + 3 - is_door;
    } else {
      line_start = x1 * 4 + 3 + is_door;
      line_end = x2 * 4 + 3 - is_door;
    }

    for (int i = line_start; i <= line_end; i++)
      for (int j = 0; j < 3; j++)
        buffer[(line_coord * ppm_width + i) * 3 + j] = color[j];

  } else {
    printf("Error writing PPM: walls should be either vertical or horizontal!\n");
    exit(EXIT_FAILURE);
  }
}

void maze_ppm (maze* m, char* filename) {
  FILE* file = fopen(filename, "w");

  ppm_width = m->room_tree->width * 4 + 7;
  ppm_height = m->room_tree->height * 4 + 7;
  fprintf(file, "P6\n%d %d\n%d\n", ppm_width, ppm_height, ppm_intensity);
  unsigned int buff_size = ppm_width * ppm_height;
  unsigned char* buffer = malloc(buff_size * 3 * sizeof(unsigned char));

  fill_buffer(ppm_back_color, buffer, buff_size);
  set_wall(ppm_wall_color, buffer, 0, 0, m->room_tree->width, 0, false);
  set_wall(ppm_wall_color, buffer, 0, m->room_tree->height, m->room_tree->width, m->room_tree->height, false);
  set_wall(ppm_wall_color, buffer, 0, 0, 0, m->room_tree->height, false);
  set_wall(ppm_wall_color, buffer, m->room_tree->width, 0, m->room_tree->width, m->room_tree->height, false);

  for (int i = 0; i < m->rooms; i++) {
    room* r = m->room_array[i];
    if (r->height > r->width) {
      set_wall(ppm_wall_color, buffer, r->x, r->door_y, r->x + r->width, r->door_y, false);
      set_wall(ppm_door_color, buffer, r->door_x, r->door_y, r->door_x - 1, r->door_y, true);
    } else {
      set_wall(ppm_wall_color, buffer, r->door_x, r->y, r->door_x, r->y + r->height, false);
      set_wall(ppm_door_color, buffer, r->door_x, r->door_y, r->door_x, r->door_y - 1, true);
    }
  }

  fwrite(buffer, sizeof(unsigned char) * 3, buff_size, file);
  fclose(file);
}
