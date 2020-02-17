#ifndef LOGIC_ABILITY_HPP
#define LOGIC_ABILITY_HPP

#include "../../omni_header.hpp"
#include <memory>
#include "../../utils/serializable.hpp"
#include "../units/unit.hpp"
#include "../../utils/coords.hpp"


class ability : public serializable {
protected:
    std::shared_ptr<coords> target_id;
    int level;
    int max_level;

public:
    ability(json &package);
    std::shared_ptr<json> pack(int serializer) override;

    virtual void update(std::shared_ptr<unit> caster) = 0;
};


#endif //LOGIC_ABILITY_HPP
