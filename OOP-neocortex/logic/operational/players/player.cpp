#include "player.hpp"

void player::process() {
    for (int i = 0; i < units->size(); ++i) {
        (*units)[i]->update();
    }
}



std::shared_ptr<std::vector<std::shared_ptr<unit>>> &player::get_units() {
    return units;
}
