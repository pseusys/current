#include <cmath>

#include "tracked.hpp"

using namespace follow_the_drow;


Tracked::Tracked(): Point() {}

Tracked::Tracked(float x, float y, float z, int frequent, float uncertain): Point(x, y, z), frequency(frequent), uncertainty(uncertain) {}

Tracked::Tracked(const Point& point, int frequent, float uncertain): Point(point), frequency(frequent), uncertainty(uncertain) {}

std::ostream& operator<<(std::ostream& stream, const Tracked& tracked) {
    return stream << "Tracked(" << tracked.x << ", " << tracked.y << ", " << tracked.z << "; " << tracked.frequency << ", " << tracked.uncertainty << ")";
}

Tracked Tracked::operator+(const Point& point) const {
    return Tracked(this->x + point.x, this->y + point.y, this->z + point.z, this->frequency, this->uncertainty);
}

Tracked Tracked::operator-(const Point& point) const {
    return Tracked(this->x - point.x, this->y - point.y, this->z - point.z, this->frequency, this->uncertainty);
}
