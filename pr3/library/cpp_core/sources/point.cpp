#include <cmath>

#include "point.hpp"

using namespace follow_the_drow;


Point::Point(): Point(0, 0, 0) {}

Point::Point(float x, float y): Point(x, y, 0) {}

Point::Point(float x, float y, float z): x(x), y(y), z(z) {}

Point::Point(const Point& point): Point(point.x, point.y, point.z) {}

Point Point::polarToCartesian(float distance, float angle) {
    float x = distance * cos(angle);
    float y = distance * sin(angle);
    return Point(x, y, 0);
}

float Point::distanceTo(const Point& point) const {
    return sqrt(pow((this->x - point.x), 2) + pow((this->y - point.y), 2));
}

Point Point::middleBetween(const Point& point) const {
    float x = (this->x + point.x) / 2;
    float y = (this->y + point.y) / 2;
    float z = (this->z + point.z) / 2;
    return Point(x, y, z);
}

std::ostream& operator<<(std::ostream& stream, const Point& point) {
    return stream << "Point(" << point.x << ", " << point.y << ", " << point.z << ")";
}

Point Point::operator+(const Point& point) const {
    return Point(this->x + point.x, this->y + point.y, this->z + point.z);
}

Point Point::operator-(const Point& point) const {
    return Point(this->x - point.x, this->y - point.y, this->z - point.z);
}
