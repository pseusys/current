//
// Created by miles on 2/10/2020.
//

#ifndef LOGIC_TILEMAP_HPP
#define LOGIC_TILEMAP_HPP

#include "../../omni_header.hpp"
#include "tiles/tile.hpp"
#include "../../utils/coords.hpp"
#include "../../utils/double_array.hpp"
#include "../../utils/serializable.hpp"


class tilemap : public serializable {
private:
    std::shared_ptr<double_array<tile>> tiles;

public:
    enum seeds {small, medium, large};

    explicit tilemap(int seed);
    explicit tilemap(json &package);
    std::shared_ptr<json> pack(int serializer) override;

    std::shared_ptr<unit>& get_unit(coords &tile);
};


#endif //LOGIC_TILEMAP_HPP
