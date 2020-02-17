//
// Created by miles on 2/10/2020.
//

#include "tilemap.hpp"

tilemap::tilemap(int seed) {
    int width, height;
    if (seed > 999) {
        width = seed / 1000;
        height = seed % 1000;
        if (width % 2 != 0) width -= 1;
        if (height % 2 != 0) height -= 1;
        if (width < 10) width = 10;
        if (height < 10) height = 10;
        if (width > 250) width = 250;
        if (height > 250) height = 250;
    } else {
        switch (seed) {
            case seeds::small:
                width = height = 50;
                break;
            case seeds::medium:
                width = height = 100;
                break;
            case seeds::large:
                width = height = 150;
                break;
            default:
                width = height = 100;
                break;
        }
    }
    log::say("Game field initialized with ", width, " width and ", height, " height");

    coords size = coords(width, height);
    this->tiles = std::make_shared<double_array<tile>>(size);
}

tilemap::tilemap(json &package) : serializable(package) {
    this->tiles = std::make_shared<double_array<tile>>(package["map"]);
}

std::shared_ptr<json> tilemap::pack(int serializer) {
    std::shared_ptr<json> package = std::make_shared<json>();
    (*package)["map"] = *((this->tiles)->pack(serializer));
    return package;
}



std::shared_ptr<unit>& tilemap::get_unit(coords& tile) {
    return (*tiles)[tile].get_placeholder();
}
