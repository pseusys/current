#ifndef LOGIC_UNIT_MOVING_HPP
#define LOGIC_UNIT_MOVING_HPP

#include "unit.hpp"


class unit_moving : public unit {
private:
    coords requested_target_id;

    std::shared_ptr<std::vector<coords>> waypoints;
    double transform;
    int moving_state;

public:
    void update() override;
};


#endif //LOGIC_UNIT_MOVING_HPP
