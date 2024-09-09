/**
 * The pivot of the robot is considered to be on level of its bottom laser, in the center.
 * Apparently, for 
*/

#include <cmath>

#include "utils.hpp"


geometry_msgs::Point cartesianToPolar(const geometry_msgs::Point point) {
    geometry_msgs::Point result;
    result.x = sqrt(pow(point.x, 2) + pow(point.y, 2));
    result.y = point.x != 0 ? atan(point.y / point.x) : 0;
    result.z = point.z;
    return result;
}

geometry_msgs::Point polarToCartesian(const geometry_msgs::Point point) {
    geometry_msgs::Point result;
    result.x = point.x * cos(point.y);
    result.y = point.x * sin(point.y);
    result.z = point.z;
    return result;
}


follow_the_drow::Point geometryToPoint(const geometry_msgs::Point point) {
    return follow_the_drow::Point(point.x, point.y, point.z);
}

geometry_msgs::Point pointToGeometry(const follow_the_drow::Point point) {
    geometry_msgs::Point result;
    result.x = point.x;
    result.y = point.y;
    result.z = point.z;
    return result;
}
