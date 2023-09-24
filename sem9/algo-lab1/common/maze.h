#ifndef MAZE_MAZE_H
#define MAZE_MAZE_H

/* One room in maze structure */
typedef struct room_t {
    unsigned int x, y, width, height, door_x, door_y;
    struct room_t* child_1, * child_2, * parent;
} room;

/* The whole maze structure */
typedef struct {
    unsigned int rooms;
    room* room_tree, ** room_array;
} maze;

/* Generate random maze */
maze* maze_random (unsigned int width, unsigned int height);

/* Free the maze */
void maze_free (maze* parent);

#endif // MAZE_MAZE_H
