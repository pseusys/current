#pragma once

#include <string>
#include <vector>

#include "follow_the_drow/point.hpp"


enum TrackerPolicy {
    First,
    Tracked,
    Closest,
    None
};

const std::string getStringFromTrackerPolicy(const TrackerPolicy policy);
const TrackerPolicy getTrackerPolicyFromString(const std::string string);

const follow_the_drow::Point track(const TrackerPolicy policy, const std::vector<follow_the_drow::Point>& points);
const follow_the_drow::Point track(const TrackerPolicy policy, const follow_the_drow::Point previous, const std::vector<follow_the_drow::Point>& points);
