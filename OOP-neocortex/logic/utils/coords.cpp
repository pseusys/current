//
// Created by miles on 2/10/2020.
//

#include "coords.hpp"


coords::coords(int x, int y) : x(x), y(y) {}

coords::coords(json &package) : serializable(package) {
    this->x = package["x"];
    this->y = package["y"];
}

std::shared_ptr<json> coords::pack(int serializer) {
    std::shared_ptr<json> package = std::make_shared<json>();
    (*package)["x"] = this->x;
    (*package)["y"] = this->y;
    return package;
}



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



void coords::reset() {
    this->x = this->y = -1;
}

bool coords::is_null() {
    return ((this->x == -1) && (this->y == -1));
}
