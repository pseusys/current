#ifndef LOGIC_PLAYER_H
#define LOGIC_PLAYER_H

#include <memory>
#include <vector>
#include "../units/unit.hpp"


class player : public serializable {
private:
    std::shared_ptr<std::vector<std::shared_ptr<unit>>> units;

public:
    player(json &package);

    std::shared_ptr<json> pack(int serializer) override;

    void process();

    std::shared_ptr<std::vector<std::shared_ptr<unit>>>& get_units();
};


#endif //LOGIC_PLAYER_H
