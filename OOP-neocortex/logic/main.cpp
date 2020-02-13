//
// Created by miles on 2/10/2020.
//

#include "utils/double_array.h"
#include "operational/game.h"

int main() {
    std::shared_ptr<game> g = game::get(0);
    std::shared_ptr<json> j = g->pack(3);
}