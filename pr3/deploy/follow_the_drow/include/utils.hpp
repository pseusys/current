#pragma once

#include "geometry_msgs/Point.h"

#include "follow_the_drow/point.hpp"


geometry_msgs::Point cartesianToPolar(const geometry_msgs::Point point);
geometry_msgs::Point polarToCartesian(const geometry_msgs::Point point);

follow_the_drow::Point geometryToPoint(const geometry_msgs::Point point);
geometry_msgs::Point pointToGeometry(const follow_the_drow::Point point);
