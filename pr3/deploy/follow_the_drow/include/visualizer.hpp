#pragma once

#include "ros/ros.h"
#include "geometry_msgs/Point.h"
#include "visualization_msgs/Marker.h"

#include "follow_the_drow/raw_data.h"
#include "follow_the_drow/detection.h"

#include "colors.hpp"


class Visualizer {
    private:
        ros::NodeHandle handle;
        ros::Subscriber rawData, annotatedData, algorithmicDetector, DROWDetector, followMe;
        ros::Publisher backVisualizer, frontVisualizer;
        const std::string frontTopic, backTopic;

        bool flattenOutput = false;
        Color bottomBackground, topBackground, annotatedBackground, centerMarker;
        Color algorithmicDetection, DROWDetection, myFollower;

        std::vector<geometry_msgs::Point> latestBottomScan, latestTopScan, latestAnnotation;
        std::vector<geometry_msgs::Point> algorithmicDetectorData, DROWDetectorData;
        geometry_msgs::Point latestFollowMe;

        bool scanReceived = false;
        bool annotatedReceived = false;
        bool algorithmicReceived = false;
        bool DROWReceived = false;
        bool followMeReceived = false;

        void rawDataCallback(const follow_the_drow::raw_data::ConstPtr& data);
        void annotatedDataCallback(const follow_the_drow::detection::ConstPtr& data);
        void algorithmicDetectorCallback(const follow_the_drow::detection::ConstPtr& data);
        void DROWDetectorCallback(const follow_the_drow::detection::ConstPtr& data);
        void followMeCallback(const geometry_msgs::Point::ConstPtr& data);

        void addPointToMarker(visualization_msgs::Marker& marker, const geometry_msgs::Point& point, const Color color) const;
        void addPointToMarker(visualization_msgs::Marker& marker, float x, float y, float z, const Color color) const;

        visualization_msgs::Marker initMarker(const std::string& topic, float scaleRadius) const;
        void update() const;

    public:
        Visualizer(Color bottom, Color top, Color annotated, Color center, Color algorithmic, Color drow, Color follower, bool flatten, const std::string& front, const std::string& back);
};
