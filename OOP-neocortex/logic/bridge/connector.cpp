#include "command.h"
#include "../other/log.h"
#include "../operational/game.h"


void init(int seed) {
    std::shared_ptr<game> g = game::get(seed);
    log::say("Game initialized.");
    return;
}

void update() {
    log::say("State updated.");
    return;
}

void connect(command signal) {
    log::say("Signal received.");
}

void destroy() {
    log::say("Game terminated.");
}
