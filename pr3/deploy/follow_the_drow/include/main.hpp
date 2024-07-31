#pragma once

#include <map>
#include <string>

#include "colors.hpp"
#include "tracker_policy.hpp"


// Node names
const std::string LIVE_LOADER = "follow_the_drow_live_loader";
const std::string FILE_LOADER = "follow_the_drow_file_loader";
const std::string VISUALIZER = "follow_the_drow_visualizer";
const std::string ALGORITHMIC_DETECTOR = "follow_the_drow_algorithmic_detector";
const std::string DROW_DETECTOR = "follow_the_drow_drow_detector";
const std::string PERSON_TRACKER = "follow_the_drow_person_tracker";

// General arguments
extern int HEARTBEAT_RATE;
extern std::string RAW_DATA_TOPIC;
extern std::string ANNOTATED_DATA_TOPIC;
extern std::string ALGORITHMIC_DETECTOR_TOPIC;
extern std::string DROW_DETECTOR_TOPIC;
extern int DATA_ANNOTATION_RATE;
extern std::string FOLLOW_ME_BEHAVIOR_TOPIC;
extern std::string GENERAL_FRAME;

void loadArgumentsForNode(int argc, char** argv, const std::string& name);

// Live loader node arguments
extern std::map<std::string, float> TRANSFORM_DATA;
extern std::string TOP_LIDAR_TOPIC;
extern std::string BOTTOM_LIDAR_TOPIC;
extern std::string ODOMETRY_TOPIC;

// Visualizer node arguments
extern bool FLATTEN_OUTPUT;
extern Color BOTTOM_BACKGROUND_COLOR;
extern Color TOP_BACKGROUND_COLOR;
extern Color ANNOTATED_BACKGROUND_COLOR;
extern Color CENTER_MARKER_COLOR;
extern Color ALGORITHMIC_DETECTION_COLOR;
extern Color DROW_DETECTION_COLOR;
extern Color FOLLOW_ME_COLOR;
extern std::string BACK_VISUALIZATION_TOPIC;
extern std::string FRONT_VISUALIZATION_TOPIC;

// Algorithmic detector node arguments
extern int FREQUENCY_INIT;
extern int FREQUENCY_MAX;
extern float UNCERTAINTY_MAX;
extern float UNCERTAINTY_MIN;
extern float UNCERTAINTY_INC;
extern float CLUSTER_THRESHOLD;
extern float DISTANCE_LEVEL;
extern float LEG_SIZE_MIN;
extern float LEG_SIZE_MAX;
extern float CHEST_SIZE_MIN;
extern float CHEST_SIZE_MAX;
extern float LEGS_DISTANCE_MIN;
extern float LEGS_DISTANCE_MAX;
extern bool ALGORITHMIC_DETECOR_VERBOSE;

// Person tracker node arguments
extern TrackerPolicy TRACKING_POLICY;
extern std::string DETECTION_INPUT_TOPIC;
