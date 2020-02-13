//
// Created by miles on 2/12/2020.
//

#include "game.h"


std::shared_ptr<game> game::g = nullptr;
int game::seeded = 0;

game::game(int seed) {
    this->map = std::make_shared<tilemap>(seed);
}



std::shared_ptr<game> game::get(int seed) {
    if (g == nullptr) g = std::shared_ptr<game>(new game(seed));
    return g;
}

game::game(json &package) : serializable(package) {
    this->map = std::make_shared<tilemap>(package["map"]);
    this->units = this->unpack_vector<unit>(package["units"]);
}

std::shared_ptr<json> game::pack(int serializer) {
    std::shared_ptr<json> package = std::make_shared<json>();
    (*package)["map"] = *(this->map)->pack(serializer);
    (*package)["units"] = *(this->pack_vector<unit>(*(this->units), serializer));
    return package;
}
