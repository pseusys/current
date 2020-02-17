#ifndef LOGIC_UNIT_H
#define LOGIC_UNIT_H

#include "../abilities/ability.hpp"
#include "../../utils/coords.hpp"
#include "../map/choreographer.hpp"


class unit : public serializable {
private:
    int id;

    std::shared_ptr<coords> pivot_position;
    int health;
    int size;

public:
    int get_id();

    explicit unit(json &package);
    std::shared_ptr<json> pack(int serializer) override;

    virtual bool is_transparent() = 0;

    std::shared_ptr<coords>& get_pivot_position();
    int get_size();

    virtual void update();
};


#endif //LOGIC_UNIT_H
