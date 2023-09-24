#include <stdio.h>
#include "svg.h"

/* Image width and height */
unsigned int svg_width, svg_height;

/* SVG drawing colors */
char* svg_back_color = "white";
char* svg_wall_color = "black";
char* svg_door_color = "burlywood";
char* svg_none_color = "none";

/* Initialize the SVG file after opening */
void svg_header (FILE* file) {
  fprintf(file, "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" width=\"%d\" height=\"%d\">\n", svg_width, svg_height);
}

/* Finalize the SVG file before closing */
void svg_footer (FILE* file) {
  fprintf(file, "</svg>\n");
}

/* Draw a line */
void svg_line (FILE* file, double x1, double y1, double x2, double y2, char* s) {
  fprintf(file, "\t<line x1=\"%lf\" y1=\"%lf\" x2=\"%lf\" y2=\"%lf\" stroke=\"%s\" style=\"stroke-width:0.5;\"/>\n", x1, y1, x2, y2, s);
}

/* Draw a rectangle */
void svg_rect (FILE* file, double x, double y, double w, double h, char* s, char* f) {
  fprintf(file, "\t<rect x=\"%lf\" y=\"%lf\" width=\"%lf\" height=\"%lf\" fill=\"%s\" stroke=\"%s\" style=\"stroke-width:0.25;\"/>\n", x, y, w, h, f, s);
}

void maze_svg (maze* m, char* filename) {
  FILE* file = fopen(filename, "w");

  svg_width = m->room_tree->width + 6;
  svg_height = m->room_tree->height + 6;
  svg_header(file);

  svg_rect(file, 0, 0, svg_width, svg_height, svg_none_color, svg_back_color);
  for (int i = 0; i < m->rooms; i++) {
    room* r = m->room_array[i];
    svg_rect(file, r->x + 3, r->y + 3, r->width, r->height, svg_wall_color, svg_none_color);
  }
  for (int i = 0; i < m->rooms; i++) {
    room* r = m->room_array[i];
    if (r->door_x == 0 && r->door_y == 0) continue;
    if (r->height > r->width) svg_line(file, r->door_x + 2.9, r->door_y + 3, r->door_x + 2.1, r->door_y + 3, svg_door_color);
    else svg_line(file, r->door_x + 3, r->door_y + 2.9, r->door_x + 3, r->door_y + 2.1, svg_door_color);
  }

  svg_footer(file);
  fclose(file);
}
