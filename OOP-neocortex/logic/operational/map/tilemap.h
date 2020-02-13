//
// Created by miles on 2/10/2020.
//

#ifndef LOGIC_TILEMAP_H
#define LOGIC_TILEMAP_H

#include "tiles/tile.h"
#include "../../utils/coords.h"
#include "../../utils/double_array.h"
#include "../../utils/serializable.h"


class tilemap : public serializable {
private:
    int width = 0, height = 0;
    std::shared_ptr<double_array<tile>> tiles;

public:
    explicit tilemap(int seed);
    explicit tilemap(json &package);
    std::shared_ptr<json> pack(int serializer) override;
};

enum seeds{small, medium, large};


#endif //LOGIC_TILEMAP_H
