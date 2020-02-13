//
// Created by miles on 2/10/2020.
//

#ifndef LOGIC_TILE_H
#define LOGIC_TILE_H

#include <memory>
#include "../../unit/unit.h"
#include "../../../utils/serializable.h"


class tile : public serializable {
private:
    int placeholder_id; // and destination
    bool passable;
    int square_pivot_anchor;
    int decoration_type;

public:
    explicit tile(bool passable = true);
    explicit tile(json &package);
    std::shared_ptr<json> pack(int serializer) override;

    int get_placeholder_id();
    void set_placeholder_id(int id);

    bool is_passable();
    bool is_occupied();

    int get_square_pivot_anchor();
    void set_square_pivot_anchor(int anchor);

    int get_decoration_type();
};


#endif //LOGIC_TILE_H
