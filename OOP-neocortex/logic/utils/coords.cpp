//
// Created by miles on 2/10/2020.
//

#include "coords.h"

coords::coords(int x, int y) : x(x), y(y) {}

int coords::get_x() {
    return x;
}

void coords::set_x(int x) {
    coords::x = x;
}

int coords::get_y() {
    return y;
}

void coords::set_y(int y) {
    coords::y = y;
}

bool coords::operator==(const coords &rhs) {
    return x == rhs.x && y == rhs.y;
}

bool coords::operator!=(coords &rhs) {
    return !(rhs == *this);
}
