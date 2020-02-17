#include "ability.hpp"


ability::ability(json &package) : serializable(package) {
    this->target_id = std::make_shared<coords>(package["target"]);
    this->level = package["level"];
}

std::shared_ptr<json> ability::pack(int serializer) {
    std::shared_ptr<json> package = std::make_shared<json>();
    (*package)["target"] = *(this->target_id->pack(serializer));
    (*package)["level"] = this->level;
    return package;
}
