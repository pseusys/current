#include "main.hpp"

#include "ros/ros.h"

#include <functional>
#include <stdexcept>


int HEARTBEAT_RATE;
const std::string HEARTBEAT_RATE_NAME = "heartbeat_rate";
std::string RAW_DATA_TOPIC;
const std::string RAW_DATA_TOPIC_NAME = "raw_data_topic";
std::string ANNOTATED_DATA_TOPIC;
const std::string ANNOTATED_DATA_TOPIC_NAME = "annotated_data_topic";
std::string ALGORITHMIC_DETECTOR_TOPIC;
const std::string ALGORITHMIC_DETECTOR_TOPIC_NAME = "algorithmic_detector_topic";
std::string DROW_DETECTOR_TOPIC;
const std::string DROW_DETECTOR_TOPIC_NAME = "drow_detector_topic";
int DATA_ANNOTATION_RATE;
const std::string DATA_ANNOTATION_RATE_NAME = "data_annotation_rate";
std::string FOLLOW_ME_BEHAVIOR_TOPIC;
const std::string FOLLOW_ME_BEHAVIOR_TOPIC_NAME = "follow_me_behavior_topic";
std::string GENERAL_FRAME;
const std::string GENERAL_FRAME_NAME = "general_frame";

std::map<std::string, float> TRANSFORM_DATA;
const std::string TRANSFORM_DATA_NAME = "transform_data";
std::string TOP_LIDAR_TOPIC;
const std::string TOP_LIDAR_TOPIC_NAME = "top_lidar_topic";
std::string BOTTOM_LIDAR_TOPIC;
const std::string BOTTOM_LIDAR_TOPIC_NAME = "bottom_lidar_topic";
std::string ODOMETRY_TOPIC;
const std::string ODOMETRY_TOPIC_NAME = "odometry_topic";

bool FLATTEN_OUTPUT;
const std::string FLATTEN_OUTPUT_NAME = "flatten_output";
Color BOTTOM_BACKGROUND_COLOR;
const std::string BOTTOM_BACKGROUND_COLOR_NAME = "bottom_backgorund_color";
Color TOP_BACKGROUND_COLOR;
const std::string TOP_BACKGROUND_COLOR_NAME = "top_backgorund_color";
Color ANNOTATED_BACKGROUND_COLOR;
const std::string ANNOTATED_BACKGROUND_COLOR_NAME = "annotated_backgorund_color";
Color CENTER_MARKER_COLOR;
const std::string CENTER_MARKER_COLOR_NAME = "center_marker_color";
Color ALGORITHMIC_DETECTION_COLOR;
const std::string ALGORITHMIC_DETECTION_COLOR_NAME = "algorithmic_detection_color";
Color DROW_DETECTION_COLOR;
const std::string DROW_DETECTION_COLOR_NAME = "drow_detection_color";
Color FOLLOW_ME_COLOR;
const std::string FOLLOW_ME_COLOR_NAME = "follow_me_color";
std::string BACK_VISUALIZATION_TOPIC;
const std::string BACK_VISUALIZATION_TOPIC_NAME = "back_visualization_topic";
std::string FRONT_VISUALIZATION_TOPIC;
const std::string FRONT_VISUALIZATION_TOPIC_NAME = "front_visualization_topic";

int FREQUENCY_INIT;
const std::string FREQUENCY_INIT_NAME = "frequency_init";
int FREQUENCY_MAX;
const std::string FREQUENCY_MAX_NAME = "frequency_max";
float UNCERTAINTY_MAX;
const std::string UNCERTAINTY_MAX_NAME = "uncertainty_max";
float UNCERTAINTY_MIN;
const std::string UNCERTAINTY_MIN_NAME = "uncertainty_min";
float UNCERTAINTY_INC;
const std::string UNCERTAINTY_INC_NAME = "uncertainty_inc";
float CLUSTER_THRESHOLD;
const std::string CLUSTER_THRESHOLD_NAME = "cluster_threshold";
float DISTANCE_LEVEL;
const std::string DISTANCE_LEVEL_NAME = "distance_level";
float LEG_SIZE_MIN;
const std::string LEG_SIZE_MIN_NAME = "leg_size_min";
float LEG_SIZE_MAX;
const std::string LEG_SIZE_MAX_NAME = "leg_size_max";
float CHEST_SIZE_MIN;
const std::string CHEST_SIZE_MIN_NAME = "chest_size_min";
float CHEST_SIZE_MAX;
const std::string CHEST_SIZE_MAX_NAME = "chest_size_max";
float LEGS_DISTANCE_MIN;
const std::string LEGS_DISTANCE_MIN_NAME = "legs_distance_min";
float LEGS_DISTANCE_MAX;
const std::string LEGS_DISTANCE_MAX_NAME = "legs_distance_max";
bool ALGORITHMIC_DETECOR_VERBOSE;
const std::string ALGORITHMIC_DETECOR_VERBOSE_NAME = "verbose";

TrackerPolicy TRACKING_POLICY;
const std::string TRACKING_POLICY_NAME = "tracking_policy";
std::string DETECTION_INPUT_TOPIC;
const std::string DETECTION_INPUT_TOPIC_NAME = "detection_input_topic";


template <typename T> T readFromParams(const std::string parameter) {
    T instance;
    if (ros::param::get(parameter, instance)) return instance;
    else throw std::runtime_error("Parameter '" + parameter + "' not defined!");
}

template <typename T> T readFromParams(const std::string parameter, std::function<T(std::string)> converter) {
    return converter(readFromParams<std::string>(parameter));
}

void loadArgumentsForNode(int argc, char** argv, const std::string& name) {
    ros::init(argc, argv, name);
    HEARTBEAT_RATE = readFromParams<int>("/" + HEARTBEAT_RATE_NAME);
    RAW_DATA_TOPIC = readFromParams<std::string>("/" + RAW_DATA_TOPIC_NAME);
    ANNOTATED_DATA_TOPIC = readFromParams<std::string>("/" + ANNOTATED_DATA_TOPIC_NAME);
    ALGORITHMIC_DETECTOR_TOPIC = readFromParams<std::string>("/" + ALGORITHMIC_DETECTOR_TOPIC_NAME);
    DROW_DETECTOR_TOPIC = readFromParams<std::string>("/" + DROW_DETECTOR_TOPIC_NAME);
    DATA_ANNOTATION_RATE = readFromParams<int>("/" + DATA_ANNOTATION_RATE_NAME);
    FOLLOW_ME_BEHAVIOR_TOPIC = readFromParams<std::string>("/" + FOLLOW_ME_BEHAVIOR_TOPIC_NAME);
    GENERAL_FRAME = readFromParams<std::string>("/" + GENERAL_FRAME_NAME);
    std::string prefix = "/" + name + "/";

    if (name == LIVE_LOADER) {
        TRANSFORM_DATA = readFromParams<std::map<std::string, float>>(prefix + TRANSFORM_DATA_NAME);
        TOP_LIDAR_TOPIC = readFromParams<std::string>(prefix + TOP_LIDAR_TOPIC_NAME);
        BOTTOM_LIDAR_TOPIC = readFromParams<std::string>(prefix + BOTTOM_LIDAR_TOPIC_NAME);
        ODOMETRY_TOPIC = readFromParams<std::string>(prefix + ODOMETRY_TOPIC_NAME);
    } else if (name == VISUALIZER) {
        FLATTEN_OUTPUT = readFromParams<bool>(prefix + FLATTEN_OUTPUT_NAME);
        BOTTOM_BACKGROUND_COLOR = readFromParams<Color>(prefix + BOTTOM_BACKGROUND_COLOR_NAME, &getColorFromString);
        TOP_BACKGROUND_COLOR = readFromParams<Color>(prefix + TOP_BACKGROUND_COLOR_NAME, &getColorFromString);
        ANNOTATED_BACKGROUND_COLOR = readFromParams<Color>(prefix + ANNOTATED_BACKGROUND_COLOR_NAME, &getColorFromString);
        CENTER_MARKER_COLOR = readFromParams<Color>(prefix + CENTER_MARKER_COLOR_NAME, &getColorFromString);
        ALGORITHMIC_DETECTION_COLOR = readFromParams<Color>(prefix + ALGORITHMIC_DETECTION_COLOR_NAME, &getColorFromString);
        DROW_DETECTION_COLOR = readFromParams<Color>(prefix + DROW_DETECTION_COLOR_NAME, &getColorFromString);
        FOLLOW_ME_COLOR = readFromParams<Color>(prefix + FOLLOW_ME_COLOR_NAME, &getColorFromString);
        BACK_VISUALIZATION_TOPIC = readFromParams<std::string>(prefix + BACK_VISUALIZATION_TOPIC_NAME);
        FRONT_VISUALIZATION_TOPIC = readFromParams<std::string>(prefix + FRONT_VISUALIZATION_TOPIC_NAME);
    } else if (name == ALGORITHMIC_DETECTOR) {
        FREQUENCY_INIT = readFromParams<int>(prefix + FREQUENCY_INIT_NAME);
        FREQUENCY_MAX = readFromParams<int>(prefix + FREQUENCY_MAX_NAME);
        UNCERTAINTY_MAX = readFromParams<float>(prefix + UNCERTAINTY_MAX_NAME);
        UNCERTAINTY_MIN = readFromParams<float>(prefix + UNCERTAINTY_MIN_NAME);
        UNCERTAINTY_INC = readFromParams<float>(prefix + UNCERTAINTY_INC_NAME);
        CLUSTER_THRESHOLD = readFromParams<float>(prefix + CLUSTER_THRESHOLD_NAME);
        DISTANCE_LEVEL = readFromParams<float>(prefix + DISTANCE_LEVEL_NAME);
        LEG_SIZE_MIN = readFromParams<float>(prefix + LEG_SIZE_MIN_NAME);
        LEG_SIZE_MAX = readFromParams<float>(prefix + LEG_SIZE_MAX_NAME);
        CHEST_SIZE_MIN = readFromParams<float>(prefix + CHEST_SIZE_MIN_NAME);
        CHEST_SIZE_MAX = readFromParams<float>(prefix + CHEST_SIZE_MAX_NAME);
        LEGS_DISTANCE_MIN = readFromParams<float>(prefix + LEGS_DISTANCE_MIN_NAME);
        LEGS_DISTANCE_MAX = readFromParams<float>(prefix + LEGS_DISTANCE_MAX_NAME);
        ALGORITHMIC_DETECOR_VERBOSE = readFromParams<bool>(prefix + ALGORITHMIC_DETECOR_VERBOSE_NAME);
    } else if (name == PERSON_TRACKER) {
        TRACKING_POLICY = readFromParams<TrackerPolicy>(prefix + TRACKING_POLICY_NAME, &getTrackerPolicyFromString);
        DETECTION_INPUT_TOPIC = readFromParams<std::string>(prefix + DETECTION_INPUT_TOPIC_NAME);
    } else throw std::runtime_error("Unknown node name '" + name + "'!");
}
