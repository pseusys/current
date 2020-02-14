#include "command.h"
#include "../other/log.h"
#include "../operational/game.h"


// is called when game is initialized;
extern "C" const char * on_start(int seed) {
    std::shared_ptr<game> g = game::get(seed);
    log::say("Game initialized.");
    return (new std::string())->data();
}

// is called on every update loop iteration;
extern "C" const char * on_update() {
    log::say("State updated.");
    return (new std::string())->data();
}

// is called on every user command, including save/load;
extern "C" const char * on_signal(const char* json_input) {
    log::say("State updated.");
    return (new std::string())->data();
}

// is called when game session finished;
extern "C" void on_destroy() {
    log::say("Game terminated.");
}
