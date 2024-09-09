#pragma once

#include <map>

#include "ros/ros.h"

#include "geometry_msgs/Point.h"
#include "sensor_msgs/LaserScan.h"
#include "nav_msgs/Odometry.h"


class LiveLoader {
    private:
        ros::NodeHandle handle;
        ros::Subscriber bottomLidar, topLidar, odometry;
        ros::Publisher rawData;

        const std::map<std::string, float> inputTransformData;
        std::array<geometry_msgs::Point, 2> bottomLidarTransform;
        std::array<geometry_msgs::Point, 2> topLidarTransform;

        bool bottomLidarInitialized = false;
        bool topLidarInitialized = false;
        bool odometryInitialized = false;

        std::vector<geometry_msgs::Point> latestBottomScan, latestTopScan;
        geometry_msgs::Point latestOdometry;

        std::vector<geometry_msgs::Point> lidarCallback(const sensor_msgs::LaserScan::ConstPtr& scan, std::array<geometry_msgs::Point, 2>& transform) const;
        void bottomLidarCallback(const sensor_msgs::LaserScan::ConstPtr& scan);
        void topLidarCallback(const sensor_msgs::LaserScan::ConstPtr& scan);
        void odometryCallback(const nav_msgs::Odometry::ConstPtr& odom);
        void update() const;
        geometry_msgs::Point setupPointFromParams(std::string paramName) const;

    public:
        LiveLoader(const std::map<std::string, float>& transformData, const std::string& topLidarTopic, const std::string& bottomLidarTopic, const std::string& odometryLidarTopic);
};
