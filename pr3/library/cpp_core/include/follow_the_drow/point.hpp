#pragma once

#include <ostream>

namespace follow_the_drow {
    class Point {
        public:
            float x, y, z;

            Point();
            Point(const Point& point);
            Point(float x, float y);
            Point(float x, float y, float z);

            static Point polarToCartesian(float distance, float angle);

            float distanceTo(const Point& point) const;
            Point middleBetween(const Point& point) const;

            friend std::ostream& operator<<(std::ostream& stream, const Point& point);
            Point operator+(const Point& point) const;
            Point operator-(const Point& point) const;
    };
}
