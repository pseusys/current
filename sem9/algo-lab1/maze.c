#include <time.h>
#include <stdlib.h>
#include "main.h"
#include "maze.h"
#include "svg.h"

/* Divides room recursively untill no more division can be done, returns total room number */
int divide_recursizve (room* root) {
    if (VERBOSE) printf("room: (x: %d, y: %d) - (w: %d, h:%d)\n", root->x, root->y, root->width, root->height);
    if (root->width == 1 || root->height == 1) return 1;

    root->child_1 = calloc(1, sizeof(room));
    root->child_1->x = root->x;
    root->child_1->y = root->y;
    root->child_1->parent = root;

    root->child_2 = calloc(1, sizeof(room));
    root->child_2->parent = root;

    int height_division = (rand() % (root->height - 1)) + 1;
    int width_division = (rand() % (root->width - 1)) + 1;
    root->door_x = root->x + width_division;
    root->door_y = root->y + height_division;

    if (VERBOSE) printf("\t has door: (x: %d, y: %d), ", root->door_x, root->door_y);

    if (root->height > root->width) {
        if (VERBOSE) printf("horizontal\n");
        root->child_2->x = root->x;
        root->child_2->y = root->y + height_division;
        root->child_1->height = height_division;
        root->child_1->width = root->child_2->width = root->width;
        root->child_2->height = root->height - height_division;
    } else {
        if (VERBOSE) printf("vertical\n");
        root->child_2->x = root->x + width_division;
        root->child_2->y = root->y;
        root->child_1->height = root->child_2->height = root->height;
        root->child_1->width = width_division;
        root->child_2->width = root->width - width_division;
    }

    return divide_recursizve(root->child_1) + divide_recursizve(root->child_2) + 1;
}

/* Collect rooms from tree structure into array structure */
int collect_rooms (unsigned int counter, room* root, room** array) {
    array[counter] = root;
    if (root->child_1 != NULL) counter = collect_rooms(counter + 1, root->child_1, array);
    if (root->child_2 != NULL) counter = collect_rooms(counter + 1, root->child_2, array);
    return counter;
}

maze* maze_random (unsigned int width, unsigned int height) {
    srand(time(NULL));
    maze* parent = calloc(1, sizeof(maze));

    parent->room_tree = calloc(1, sizeof(room));
    parent->room_tree->width = width;
    parent->room_tree->height = height;

    parent->rooms = divide_recursizve(parent->room_tree);
    parent->room_array = malloc(parent->rooms * sizeof(room*));
    collect_rooms(0, parent->room_tree, parent->room_array);

    return parent;
}

void maze_free (maze* parent) {
    for (int i = 0; i < parent->rooms; i++) free(parent->room_array[i]);
    free(parent->room_array);
    free(parent);
}
