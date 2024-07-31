#include <algorithm>

#include "main.hpp"
#include "utils.hpp"
#include "visualizer.hpp"

#define BACKGROUND_RADIUS 0.05
#define DETECTION_RADIUS 0.15


void Visualizer::rawDataCallback(const follow_the_drow::raw_data::ConstPtr& data) {
    latestBottomScan = std::vector<geometry_msgs::Point>(data->bottom_lidar.size());
    for (int i = 0; i < data->bottom_lidar.size(); i++)
        latestBottomScan[i] = polarToCartesian(data->bottom_lidar[i]);

    latestTopScan = std::vector<geometry_msgs::Point>(data->top_lidar.size());
    for (int i = 0; i < data->top_lidar.size(); i++)
        latestTopScan[i] = polarToCartesian(data->top_lidar[i]);

    scanReceived = true;
}

void Visualizer::annotatedDataCallback(const follow_the_drow::detection::ConstPtr& data) {
    latestAnnotation = data->detection;
    annotatedReceived = true;
}

void Visualizer::algorithmicDetectorCallback(const follow_the_drow::detection::ConstPtr& data) {
    algorithmicDetectorData = data->detection;
    algorithmicReceived = true;
}

void Visualizer::DROWDetectorCallback(const follow_the_drow::detection::ConstPtr& data) {
    DROWDetectorData = data->detection;
    DROWReceived = true;
}

void Visualizer::followMeCallback(const geometry_msgs::Point::ConstPtr& data) {
    latestFollowMe.x = data->x;
    latestFollowMe.y = data->y;
    followMeReceived = true;
}

void Visualizer::addPointToMarker(visualization_msgs::Marker& marker, const geometry_msgs::Point& point, const Color color) const {
    geometry_msgs::Point copy = point;
    if (flattenOutput) copy.z = 0;
    marker.points.push_back(copy);
    marker.colors.push_back(getRGBAFromColor(color));
}

void Visualizer::addPointToMarker(visualization_msgs::Marker& marker, float x, float y, float z, const Color color) const {
    geometry_msgs::Point point;
    point.x = x;
    point.y = y;
    point.z = z;
    addPointToMarker(marker, point, color);
}

visualization_msgs::Marker Visualizer::initMarker(const std::string& topic, float scaleRadius) const {
    visualization_msgs::Marker marker;

    marker.header.frame_id = GENERAL_FRAME;
    marker.header.stamp = ros::Time::now();
    marker.ns = topic;
    marker.id = 0;
    marker.type = visualization_msgs::Marker::POINTS;
    marker.action = visualization_msgs::Marker::ADD;

    marker.pose.orientation.w = 1;
    marker.scale.x = scaleRadius;
    marker.scale.y = scaleRadius;
    marker.color.a = 1.0;

    return marker;
}

void Visualizer::update() const {
    if (scanReceived) {
        visualization_msgs::Marker backMarker = initMarker(backTopic, BACKGROUND_RADIUS);

        for (int loop = 0; loop < latestBottomScan.size(); loop++)
            addPointToMarker(backMarker, latestBottomScan[loop], bottomBackground);
        for (int loop = 0; loop < latestTopScan.size(); loop++)
            addPointToMarker(backMarker, latestTopScan[loop], topBackground);

        backVisualizer.publish(backMarker);
    }

    visualization_msgs::Marker frontMarker = initMarker(frontTopic, DETECTION_RADIUS);

    if (annotatedReceived)
        for (int loop = 0; loop < latestAnnotation.size(); loop++)
            addPointToMarker(frontMarker, latestAnnotation[loop], annotatedBackground);

    if (algorithmicReceived)
        for (int loop = 0; loop < algorithmicDetectorData.size(); loop++)
            addPointToMarker(frontMarker, algorithmicDetectorData[loop], algorithmicDetection);

    if (DROWReceived)
        for (int loop = 0; loop < DROWDetectorData.size(); loop++)
            addPointToMarker(frontMarker, DROWDetectorData[loop], DROWDetection);

    if (followMeReceived)
        addPointToMarker(frontMarker, latestFollowMe, myFollower);

    addPointToMarker(frontMarker, 0, 0, 0, centerMarker);
    frontVisualizer.publish(frontMarker);
}

Visualizer::Visualizer(Color bottom, Color top, Color annotated, Color center, Color algorithmic, Color drow, Color follower, bool flatten, const std::string& front, const std::string& back): bottomBackground(bottom), topBackground(top), annotatedBackground(annotated), centerMarker(center), algorithmicDetection(algorithmic), DROWDetection(drow), myFollower(follower), flattenOutput(flatten), frontTopic(front), backTopic(back) {
    backVisualizer = handle.advertise<visualization_msgs::Marker>(backTopic, 1);
    frontVisualizer = handle.advertise<visualization_msgs::Marker>(frontTopic, 1);
    rawData = handle.subscribe(RAW_DATA_TOPIC, 1, &Visualizer::rawDataCallback, this);
    annotatedData = handle.subscribe(ANNOTATED_DATA_TOPIC, 1, &Visualizer::annotatedDataCallback, this);
    algorithmicDetector = handle.subscribe(ALGORITHMIC_DETECTOR_TOPIC, 1, &Visualizer::algorithmicDetectorCallback, this);
    DROWDetector = handle.subscribe(DROW_DETECTOR_TOPIC, 1, &Visualizer::DROWDetectorCallback, this);
    followMe = handle.subscribe(FOLLOW_ME_BEHAVIOR_TOPIC, 1, &Visualizer::followMeCallback, this);

    ros::Rate rate(HEARTBEAT_RATE);
    while (ros::ok()) {
        ros::spinOnce();
        update();
        rate.sleep();
    }
}


int main(int argc, char **argv) {
    loadArgumentsForNode(argc, argv, VISUALIZER);
    Visualizer bsObject(BOTTOM_BACKGROUND_COLOR, TOP_BACKGROUND_COLOR, ANNOTATED_BACKGROUND_COLOR, CENTER_MARKER_COLOR, ALGORITHMIC_DETECTION_COLOR, DROW_DETECTION_COLOR, FOLLOW_ME_COLOR, FLATTEN_OUTPUT, FRONT_VISUALIZATION_TOPIC, BACK_VISUALIZATION_TOPIC);
    ros::spin();
    return 0;
}
