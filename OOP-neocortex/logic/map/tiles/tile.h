//
// Created by miles on 2/10/2020.
//

#ifndef LOGIC_TILE_H
#define LOGIC_TILE_H


class tile {
private:
    int placeholder_id;
    bool passable;
    int square_pivot_anchor;
    int decoration_type;

public:
    bool is_empty();

    int get_placeholder_id();

    bool is_passable();

    int get_square_pivot_anchor();

    int get_decoration_type();
};


#endif //LOGIC_TILE_H
