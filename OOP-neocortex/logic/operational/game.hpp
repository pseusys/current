#ifndef LOGIC_GAME_HPP
#define LOGIC_GAME_HPP

#include "players/player.hpp"
#include "map/tilemap.hpp"
#include <stack>


class game : public serializable {
private:
    static std::shared_ptr<game> g;
    static int seeded;
    explicit game(int seed);
    explicit game(json &package);

    std::vector<unit> units;
    int filled_until;
    std::stack<int> units_pool;
    int upper_id;

    std::shared_ptr<tilemap> map;
    std::shared_ptr<std::vector<player>> players;

public:
    static std::shared_ptr<game> get(int seed = seeded);
    static std::shared_ptr<game> reset(json &package);
    std::shared_ptr<json> pack(int serializer) override;

    std::shared_ptr<unit> get_unit_by_id(int id);
};

enum serialization_mods{pre_game, on_game, post_game};


#endif //LOGIC_GAME_HPP
