#ifndef LOGIC_GAME_H
#define LOGIC_GAME_H

#include "unit/unit.h"
#include "map/tilemap.h"


class game : public serializable {
private:
    static std::shared_ptr<game> g;
    static int seeded;
    explicit game(int seed);

    std::shared_ptr<tilemap> map;
    std::shared_ptr<std::vector<unit>> units;

public:
    static std::shared_ptr<game> get(int seed = seeded);
    explicit game(json &package);
    std::shared_ptr<json> pack(int serializer) override;
};

enum serialization_mods{pre_game, on_game, post_game};


#endif //LOGIC_GAME_H
