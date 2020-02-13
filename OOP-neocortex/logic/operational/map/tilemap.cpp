//
// Created by miles on 2/10/2020.
//

#include "tilemap.h"

tilemap::tilemap(int seed) {
    if (seed > 999) {
        this->width = seed / 1000;
        this->height = seed % 1000;
        if (this->width % 2 != 0) this->width -= 1;
        if (this->height % 2 != 0) this->height -= 1;
        if (this->width < 10) this->width = 10;
        if (this->height < 10) this->height = 10;
        if (this->width > 250) this->width = 250;
        if (this->height > 250) this->height = 250;
    } else {
        switch (seed) {
            case seeds::small:
                this->width = this->height = 50;
                break;
            case seeds::medium:
                this->width = this->height = 100;
                break;
            case seeds::large:
                this->width = this->height = 150;
                break;
            default:
                this->width = this->height = 100;
                break;
        }
    }
    log::say("Game field initialized with ", this->width, " width and ", this->height, " height");

    coords size = coords(this->width, this->height);
    this->tiles = std::make_shared<double_array<tile>>(size);
}

tilemap::tilemap(json &package) : serializable(package) {
    this->width = package["width"];
    this->height = package["height"];
    this->tiles = std::make_shared<double_array<tile>>(package["map"]);
}

std::shared_ptr<json> tilemap::pack(int serializer) {
    std::shared_ptr<json> package = std::make_shared<json>();
    (*package)["width"] = this->width;
    (*package)["height"] = this->height;
    (*package)["map"] = *((this->tiles)->pack(serializer));
    return package;
}
