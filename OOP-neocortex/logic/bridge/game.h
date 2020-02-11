/**
 * Exported!
 * This sign indicates that this class (or some of its parts) should be exported to c# project part.
 * Consequences in language complexity are expected.
 * By default all contents considered to be exported,
 * if other is not marked by special "exported" and "unexported" signs.
 */

#ifndef LOGIC_GAME_H
#define LOGIC_GAME_H

#include <cstdlib>
#include "../ids/constants.h"
#include "../map/tilemap.h"
#include "../utils/containers/list.h"
#include "../other/log.h"
#include "../unit/choreographer.h"

class transferable {
public:
    int width;
    int height;
    int** textures;

    int count;
    int* units;

    transferable(int width, int height);
};

class logic {
private:
    transferable* bridge;
    tilemap* map;
    choreographer* units;

public:
    explicit logic(transferable *bridge);
};

class game {
private:
    logic* logi;
public:
    transferable* trans;

    explicit game(int seed);
};

enum seeds {small, medium, large};


#endif //LOGIC_GAME_H
