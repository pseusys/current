#include "command.h"
#include "../other/log.h"
#include "../operational/game.h"


extern "C" __declspec(dllexport) void init(int seed) {
    std::shared_ptr<game> g = game::get(seed);
    log::say("Game initialized.");
    return;
}

extern "C" __declspec(dllexport) void update() {
    log::say("State updated.");
    return;
}

extern "C" __declspec(dllexport) void connect(command signal) {
    log::say("Signal received.");
}

extern "C" __declspec(dllexport) void destroy() {
    log::say("Game terminated.");
}
