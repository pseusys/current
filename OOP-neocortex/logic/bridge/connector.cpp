#include "command.hpp"
#include "../other/log.hpp"
#include "../operational/game.hpp"


// is called when game is initialized;
extern "C" std::string* on_start(int seed) {
    std::shared_ptr<game> g = game::get(seed);
    log::say("Game initialized.");
    return new std::string();
}

// is called on every update loop iteration;
extern "C" std::vector<std::string>* on_update() {
    log::say("State updated.");
    return new std::vector<std::string>();
}

// is called on every user command, including save/load;
extern "C" std::string* on_signal(const char* json_input) {
    log::say("State updated.");
    return new std::string();
}

// is called when game session finished;
extern "C" void on_destroy() {
    log::say("Game terminated.");
}
