//
// Created by miles on 2/12/2020.
//

#include "game.hpp"


std::shared_ptr<game> game::g = nullptr;
int game::seeded = 0;

game::game(int seed) {
    this->map = std::make_shared<tilemap>(seed);
    this->players = std::make_shared<std::vector<player>>();
    choreographer::init(this->map);
}



std::shared_ptr<game> game::get(int seed) {
    if (g == nullptr) g = std::shared_ptr<game>(new game(seed));
    return g;
}

std::shared_ptr<game> game::reset(json &package) {
    g = std::shared_ptr<game>(new game(package));
    return g;
}

game::game(json &package) : serializable(package) {
    this->map = std::make_shared<tilemap>(package["tilemap"]);
    this->players = this->unpack_vector<player>(package["players"]);
    choreographer::init(this->map);
}

std::shared_ptr<json> game::pack(int serializer) {
    std::shared_ptr<json> package = std::make_shared<json>();
    (*package)["tilemap"] = *(this->map)->pack(serializer);
    (*package)["players"] = *(this->pack_vector<player>(*(this->players), serializer));
    return package;
}



std::shared_ptr<unit> game::get_unit_by_id(int id) {
    for (int i = 0; i < players->size(); ++i) {
        for (int j = 0; j < (*players)[i].get_units()->size(); ++j) {
            std::shared_ptr<unit> current = (*(*players)[i].get_units())[j];
            if (current->get_id() == id) return current;
        }
    }
    return nullptr;
}
