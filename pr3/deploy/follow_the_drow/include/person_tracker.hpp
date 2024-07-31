#pragma once

#include "ros/ros.h"

#include "geometry_msgs/Point.h"
#include "follow_the_drow/detection.h"

#include "tracker_policy.hpp"


class PersonTracker {
    private:
        ros::NodeHandle handle;
        ros::Subscriber detection;
        ros::Publisher personPosition;

        TrackerPolicy policy;
        bool detectionInitialized = false;

        std::string inputTopic;
        std::vector<follow_the_drow::Point> latestDetection;
        follow_the_drow::Point person;
        bool personTracked = false;

        void detectionCallback(const follow_the_drow::detection::ConstPtr& data);
        void update();

    public:
        PersonTracker(TrackerPolicy tracker, std::string input);
};
