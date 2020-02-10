//
// Created by miles on 2/10/2020.
//

#include "../gm/game.h"
#include "command.h"

extern "C" __declspec(dllexport) game* init() {
    game* g = new game();
    return g;
}

extern "C" __declspec(dllexport) void update(game* g) {

}

extern "C" __declspec(dllexport) void connect(game* g, command signal) {

}

extern "C" __declspec(dllexport) void destroy(game* g) {
    delete g;
}
