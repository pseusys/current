#include "unit_moving.hpp"
#include "../map/choreographer.hpp"


void unit_moving::update() {
    unit::update();
    if (requested_target_id.is_null()) {
        moving_state = choreographer::movement_state::moving;
        waypoints = choreographer::get()->find_way(std::shared_ptr<unit_moving>(this), std::make_shared<coords>(requested_target_id));
        requested_target_id.reset();
    }

    if ((moving_state == choreographer::movement_state::waiting) || (moving_state == choreographer::movement_state::moving)) {
        if (waypoints->empty()) {
            moving_state = choreographer::movement_state::still;
            return;
        }
        moving_state = choreographer::get()->can_move(std::shared_ptr<unit_moving>(this), std::make_shared<coords>((*waypoints)[0]));

        if (moving_state == choreographer::movement_state::moving) {
            choreographer::get()->move(std::shared_ptr<unit_moving>(this), std::make_shared<coords>((*waypoints)[0]));
            waypoints->erase(waypoints->begin());
        } else if (moving_state == choreographer::movement_state::still) {
            waypoints = choreographer::get()->find_way(std::shared_ptr<unit_moving>(this), std::make_shared<coords>((*waypoints)[waypoints->size() - 1]));
            moving_state = choreographer::movement_state::moving;
            this->update();
        }
    }
}