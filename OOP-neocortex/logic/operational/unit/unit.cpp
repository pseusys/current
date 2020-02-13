//
// Created by miles on 2/11/2020.
//

#include "unit.h"


unit::unit(json &package) : serializable(package) {
    this->health = std::make_shared<ability>(package["health"]);
    this->abilities = unpack_vector<ability>(package["abilities"]);
}

std::shared_ptr<json> unit::pack(int serializer) {
    std::shared_ptr<json> package = std::make_shared<json>();
    (*package)["health"] = *(this->health->pack(serializer));
    (*package)["abilities"] = *(pack_vector(*(this->abilities), serializer));
    return std::shared_ptr<json>();
}
