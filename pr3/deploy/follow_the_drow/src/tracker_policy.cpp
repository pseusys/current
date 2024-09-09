#include <stdexcept>

#include "follow_the_drow/misc.hpp"

#include "tracker_policy.hpp"


const std::string getStringFromTrackerPolicy(const TrackerPolicy policy) {
    switch (policy) {
        case TrackerPolicy::First: return "first";
        case TrackerPolicy::Tracked: return "tracked";
        case TrackerPolicy::Closest: return "closest";
        case TrackerPolicy::None: return "none";
        default: throw std::runtime_error("Bad tracker policy value!");
    }
}

const TrackerPolicy getTrackerPolicyFromString(const std::string string) {
    if (string == "first") return TrackerPolicy::First;
    if (string == "tracked") return TrackerPolicy::Tracked;
    if (string == "closest") return TrackerPolicy::Closest;
    if (string == "none") return TrackerPolicy::None;
    throw std::runtime_error("Bad tracker policy name '" + string + "'!");
}


const follow_the_drow::Point trackFirst(const std::vector<follow_the_drow::Point>& points) {
    return points[0];
}

const follow_the_drow::Point trackTracked(const std::vector<follow_the_drow::Point>& points, const follow_the_drow::Point previous) {
    double minDist = previous.distanceTo(points[0]);
    follow_the_drow::Point tracked = points[0];
    for (follow_the_drow::Point point: points) if (previous.distanceTo(point) < minDist) {
        minDist = previous.distanceTo(point);
        tracked = point;
    }
    return tracked;
}

const follow_the_drow::Point trackClosest(const std::vector<follow_the_drow::Point>& points) {
    return trackTracked(points, follow_the_drow::Point(0, 0));
}

const follow_the_drow::Point track(const TrackerPolicy policy, const std::vector<follow_the_drow::Point>& points) {
    follow_the_drow::Point zero = follow_the_drow::Point(0, 0);
    return track(policy, zero, points);
}

const follow_the_drow::Point track(const TrackerPolicy policy, const follow_the_drow::Point previous, const std::vector<follow_the_drow::Point>& points) {
    if (points.size() < 1) throw std::runtime_error("Points vector is empty!");
    switch (policy) {
        case TrackerPolicy::First: return trackFirst(points);
        case TrackerPolicy::Tracked: return trackTracked(points, previous);
        case TrackerPolicy::Closest: return trackClosest(points);
        case TrackerPolicy::None: return follow_the_drow::Point(0, 0);
        default: throw std::runtime_error("Bad tracker policy value!");
    }
}
