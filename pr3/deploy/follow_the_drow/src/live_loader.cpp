#include "live_loader.hpp"

#include "tf/transform_listener.h"

#include "follow_the_drow/raw_data.h"

#include "utils.hpp"
#include "main.hpp"

#define POLAR 0
#define CARTESIAN 1


std::vector<geometry_msgs::Point> LiveLoader::lidarCallback(const sensor_msgs::LaserScan::ConstPtr& scan, std::array<geometry_msgs::Point, 2>& transform) const {
    int nbBeams = ((-1 * scan->angle_min) + scan->angle_max) / scan->angle_increment;
    std::vector<geometry_msgs::Point> latestScan(nbBeams);

    float beamAngle = scan->angle_min;
    for (int loop = 0; loop < nbBeams; loop++, beamAngle += scan->angle_increment) {
        bool measureOutOfRange = (scan->ranges[loop] < scan->range_max) && (scan->ranges[loop] > scan->range_min);
        float patchedMeasure = measureOutOfRange ? scan->ranges[loop] : scan->range_max;

        latestScan[loop].x = transform[POLAR].x + transform[CARTESIAN].x + patchedMeasure;
        latestScan[loop].y = transform[POLAR].y + transform[CARTESIAN].y + beamAngle;
        latestScan[loop].z = transform[POLAR].z + transform[CARTESIAN].z;
    }
    return latestScan;
}

void LiveLoader::bottomLidarCallback(const sensor_msgs::LaserScan::ConstPtr& scan) {
    latestBottomScan = lidarCallback(scan, bottomLidarTransform);
    bottomLidarInitialized = true;
}

void LiveLoader::topLidarCallback(const sensor_msgs::LaserScan::ConstPtr& scan) {
    latestTopScan = lidarCallback(scan, topLidarTransform);
    topLidarInitialized = true;
}

void LiveLoader::odometryCallback(const nav_msgs::Odometry::ConstPtr& odom) {
    latestOdometry.x = odom->pose.pose.position.x;
    latestOdometry.y = odom->pose.pose.position.y;
    latestOdometry.z = tf::getYaw(odom->pose.pose.orientation);
    odometryInitialized = true;
}

void LiveLoader::update() const {
    if (!bottomLidarInitialized || !topLidarInitialized) return;
    follow_the_drow::raw_data message;
    message.bottom_lidar = latestBottomScan;
    message.top_lidar = latestTopScan;
    message.odometry = latestOdometry;
    rawData.publish(message);
}

geometry_msgs::Point LiveLoader::setupPointFromParams(std::string paramName) const {
    geometry_msgs::Point point;
    point.x = inputTransformData.at(paramName + "_x");
    point.y = inputTransformData.at(paramName + "_y");
    point.z = inputTransformData.at(paramName + "_z");
    return point;
}

LiveLoader::LiveLoader(const std::map<std::string, float>& transformData, const std::string& topLidarTopic, const std::string& bottomLidarTopic, const std::string& odometryLidarTopic): inputTransformData(transformData) {
    bottomLidar = handle.subscribe(bottomLidarTopic, 1, &LiveLoader::bottomLidarCallback, this);
    topLidar = handle.subscribe(topLidarTopic, 1, &LiveLoader::topLidarCallback, this);
    odometry = handle.subscribe(odometryLidarTopic, 1, &LiveLoader::odometryCallback, this);
    rawData = handle.advertise<follow_the_drow::raw_data>(RAW_DATA_TOPIC, 1);

    bottomLidarTransform = {cartesianToPolar(setupPointFromParams("bottom_polar")), setupPointFromParams("bottom_cartesian")};
    topLidarTransform = {cartesianToPolar(setupPointFromParams("top_polar")), setupPointFromParams("top_cartesian")};

    ros::Rate rate(HEARTBEAT_RATE);
    while (ros::ok()) {
        ros::spinOnce();
        update();
        rate.sleep();
    }
}


int main(int argc, char **argv) {
    loadArgumentsForNode(argc, argv, LIVE_LOADER);
    LiveLoader bsObject(TRANSFORM_DATA, TOP_LIDAR_TOPIC, BOTTOM_LIDAR_TOPIC, ODOMETRY_TOPIC);
    ros::spin();
    return 0;
}
