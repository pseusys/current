#pragma once

#include <ostream>

#include "point.hpp"

namespace follow_the_drow {
    class Tracked: public Point {
        public:
            int frequency;
            float uncertainty;

            Tracked();
            Tracked(float x, float y, float z, int frequent, float uncertain);
            Tracked(const Point& point, int frequent, float uncertain);

            friend std::ostream& operator<<(std::ostream& stream, const Tracked& tracked);
            Tracked operator+(const Point& point) const;
            Tracked operator-(const Point& point) const;
    };
}
