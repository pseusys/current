#ifndef LOGIC_CHOREOGRAPHER_HPP
#define LOGIC_CHOREOGRAPHER_HPP

#include <memory>
#include "../units/unit_moving.hpp"
#include "../../utils/coords.hpp"
#include "../units/unit_moving.hpp"
#include "tilemap.hpp"


class choreographer {
private:
    static std::shared_ptr<choreographer> instance;
    std::shared_ptr<tilemap> map;

public:
    enum movement_state {still, waiting, moving};
    enum movement_priority {sedative = 0, high, medium, low};

    static void init(std::shared_ptr<tilemap>& tilemap);
    static std::shared_ptr<choreographer> get();

    int can_move(std::shared_ptr<unit_moving> mover, std::shared_ptr<coords> destination);
    void move(std::shared_ptr<unit_moving> unit, std::shared_ptr<coords> destination);
    std::shared_ptr<std::vector<coords>> find_way(std::shared_ptr<unit_moving> unit, std::shared_ptr<coords> destination);

    void kill(std::shared_ptr<unit> unit);
};


#endif //LOGIC_CHOREOGRAPHER_HPP
