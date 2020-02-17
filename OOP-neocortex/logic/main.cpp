//
// Created by miles on 2/10/2020.
//

#include "utils/double_array.hpp"
#include "operational/game.hpp"

int main() {
    std::shared_ptr<game> g = game::get(0);
    std::shared_ptr<json> j = g->pack(3);
    std::string s = j->dump(3);
    std::cout << s << std::endl;
    std::shared_ptr<game> gg = game::reset(*j);
}