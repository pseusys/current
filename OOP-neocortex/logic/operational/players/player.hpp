#ifndef LOGIC_PLAYER_H
#define LOGIC_PLAYER_H


#include <memory>
#include <vector>
#include "../units/unit.hpp"

class player {
private:
    std::shared_ptr<std::vector<std::shared_ptr<unit>>> units;

public:
    void process();

    std::shared_ptr<std::vector<std::shared_ptr<unit>>>& get_units();
};


#endif //LOGIC_PLAYER_H
