//
// Created by miles on 2/10/2020.
//

#include "tile.h"


bool tile::is_empty() {
    return placeholder_id == 0;
}

int tile::get_placeholder_id() {
    return placeholder_id;
}

bool tile::is_passable() {
    return passable;
}

int tile::get_square_pivot_anchor() {
    return square_pivot_anchor;
}

int tile::get_decoration_type() {
    return decoration_type;
}
