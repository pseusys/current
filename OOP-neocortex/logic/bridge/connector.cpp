//
// Created by miles on 2/10/2020.
//

#include "game.h"
#include "command.h"
#include "../other/log.h"

extern "C" __declspec(dllexport) game* init(int seed) {
    game* g = new game(seed);
    log::say("Game initialized.");
    return g;
}

extern "C" __declspec(dllexport) void update(game* g) {
    log::say("State updated.");
}

extern "C" __declspec(dllexport) void connect(game* g, command signal) {
    log::say("Signal received.");
}

extern "C" __declspec(dllexport) void destroy(game* g) {
    log::say("Game terminated.");
    delete g;
}
