#include "unit.hpp"


int unit::get_id() {
    return id;
}



unit::unit(json &package) : serializable(package) {
}

std::shared_ptr<json> unit::pack(int serializer) {
    std::shared_ptr<json> package = std::make_shared<json>();
    return std::shared_ptr<json>();
}



std::shared_ptr<coords>& unit::get_pivot_position() {
    return pivot_position;
}

int unit::get_size() {
    return size;
}



void unit::update() {
    if (health <= 0) choreographer::get()->kill(std::shared_ptr<unit>(this));
}
