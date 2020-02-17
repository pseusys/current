//
// Created by miles on 2/10/2020.
//

#include "tile.hpp"


tile::tile(bool passable) : passable(passable) {

}

tile::tile(json &package) : serializable(package) {
    this->placeholder_id = package["placeholder_id"];
    this->passable = package["passable"];
    this->square_pivot_anchor = package["square_pivot_anchor"];
    this->decoration_type = package["decoration_type"];
}

std::shared_ptr<json> tile::pack(int serializer) {
    std::shared_ptr<json> package = std::make_shared<json>();
    (*package)["placeholder_id"] = this->placeholder_id;
    (*package)["passable"] = this->passable;
    (*package)["square_pivot_anchor"] = this->square_pivot_anchor;
    (*package)["decoration_type"] = this->decoration_type;
    return package;
}



int tile::get_placeholder_id() {
    return placeholder_id;
}

void tile::set_placeholder_id(int id) {
    this->placeholder_id = id;
}



std::shared_ptr<unit> tile::get_placeholder() {
    return game::get()->get_unit_by_id(placeholder_id);
}



bool tile::is_passable() {
    return passable;
}

bool tile::is_occupied() {
    return this->placeholder_id == 0;
}



int tile::get_square_pivot_anchor() {
    return square_pivot_anchor;
}

void tile::set_square_pivot_anchor(int anchor) {
    this->square_pivot_anchor = anchor;
}



int tile::get_decoration_type() {
    return decoration_type;
}
