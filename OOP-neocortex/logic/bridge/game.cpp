#include "game.h"

game::game(int seed) {
    int field_width, field_height;

    if (seed > 999) {
        field_width = seed / 1000;
        field_height = seed % 1000;
        if (field_width % 2 != 0) field_width -= 1;
        if (field_height % 2 != 0) field_height -= 1;
        if (field_width < 10) field_width = 10;
        if (field_height < 10) field_height = 10;
        if (field_width > 250) field_width = 250;
        if (field_height > 250) field_height = 250;
    } else {
        switch (seed) {
            case seeds::small:
                field_width = field_height = 50;
                break;
            case seeds::medium:
                field_width = field_height = 100;
                break;
            case seeds::large:
                field_width = field_height = 150;
                break;
            default:
                field_width = field_height = 100;
                break;
        }
    }
    log::say("Game field initialized with ", field_width, " width and ", field_height, " height");

    this->trans = new transferable(field_width, field_height);
    this->logi = new logic(trans);
}

transferable::transferable(int width, int height) : width(width), height(height) {
    this->textures = (int**) calloc(height, sizeof(int*));
    for (int i = 0; i < height; ++i) {
        this->textures[i] = (int*) calloc(width, sizeof(int));
    }

    this->count = 0;
    this->units = (int*) calloc(3 * player_units_limit + neutral_units_limit, sizeof(int));
}

logic::logic(transferable *bridge) : bridge(bridge) {

}
