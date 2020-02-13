#ifndef LOGIC_ABILITY_H
#define LOGIC_ABILITY_H

#include <memory>
#include "../../utils/serializable.h"


class ability : public serializable {
private:
    int target_id;
    bool is_unit_targeted;

    int level;
    int max_level;

public:
    ability(json &package);
    std::shared_ptr<json> pack(int serializer) override;

    void update();
};


#endif //LOGIC_ABILITY_H
