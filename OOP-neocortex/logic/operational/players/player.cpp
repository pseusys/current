#include "player.hpp"


player::player(json &package) : serializable(package) {}

std::shared_ptr<json> player::pack(int serializer) {
    return std::shared_ptr<json>();
}



void player::process() {
    for (int i = 0; i < units->size(); ++i) {
        (*units)[i]->update();
    }
}



std::shared_ptr<std::vector<std::shared_ptr<unit>>> &player::get_units() {
    return units;
}
