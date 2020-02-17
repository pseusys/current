//
// Created by miles on 2/10/2020.
//

#ifndef LOGIC_TILE_HPP
#define LOGIC_TILE_HPP

#include "../../../omni_header.hpp"
#include <memory>
#include "../../units/unit.hpp"
#include "../../../utils/serializable.hpp"
#include "../../game.hpp"


class tile : public serializable {
private:
    int placeholder_id;
    bool passable;
    int square_pivot_anchor;
    int decoration_type;

public:
    explicit tile(bool passable = true);
    explicit tile(json &package);
    std::shared_ptr<json> pack(int serializer) override;

    int get_placeholder_id();
    void set_placeholder_id(int id);

    std::shared_ptr<unit> get_placeholder();

    bool is_passable();
    bool is_occupied();

    int get_square_pivot_anchor();
    void set_square_pivot_anchor(int anchor);

    int get_decoration_type();
};


#endif //LOGIC_TILE_HPP
