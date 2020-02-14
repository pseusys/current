//
// Created by miles on 2/11/2020.
//

#ifndef LOGIC_UNIT_H
#define LOGIC_UNIT_H

#include "../abilities/ability.h"


class unit : public serializable {
private:
    std::shared_ptr<ability> health;
    std::shared_ptr<std::vector<ability>> abilities;

public:
    explicit unit(json &package);
    std::shared_ptr<json> pack(int serializer) override;
};


#endif //LOGIC_UNIT_H
