#include "ros/ros.h"
#include <geometry_msgs/Twist.h>
#include "geometry_msgs/Point.h"
#include "geometry_msgs/PoseWithCovarianceStamped.h"
#include "geometry_msgs/Quaternion.h"
#include "visualization_msgs/Marker.h"
#include "geometry_msgs/Point.h"
#include "std_msgs/ColorRGBA.h"
#include "nav_msgs/Odometry.h"
#include "std_msgs/String.h"
#include "std_msgs/Float32.h"
#include <cmath>
#include <tf/transform_datatypes.h>
#include "std_msgs/Bool.h"

#define waiting_for_a_person 0
#define observing_the_person 1
#define rotating_to_the_person 2
#define moving_to_the_person 3
#define interacting_with_the_person 4
#define rotating_to_the_base 5
#define moving_to_the_base 6
#define resetting_orientation 7

#define frequency_expected 25
#define max_base_distance 6

#define rotation_epsilon M_PI/18
#define approach_epsilon 0.3
#define translation_epsilon 0.1
#define interaction_epsilon 1.5

#define person_moving_epsilon 0.025
#define float_comparison_precision 0.0000001

class decision_node
{
private:
    ros::NodeHandle n;
    ros::Publisher pub_datmo_marker;

    // communication with datmo_node
    ros::Subscriber sub_person_position;
    bool new_person_position, person_tracked, person_moving;
    geometry_msgs::Point person_position, last_person;

    // communication with robot_moving_node
    ros::Subscriber sub_robot_moving;
    bool robot_moving;

    // communication with rotation_action
    ros::Publisher pub_rotation_to_do;
    float rotation_to_person;

    // communication with action_node
    ros::Publisher pub_goal_to_reach;
    float translation_to_person;

    // communication with localization
    ros::Subscriber sub_localization;
    bool new_localization;
    bool init_localization;
    geometry_msgs::Point current_position;
    float current_orientation;
    float translation_to_base;
    float rotation_to_base;
    geometry_msgs::Point local_base_position;

    int current_state, previous_state;
    int frequency;
    geometry_msgs::Point base_position;
    float base_orientation;
    geometry_msgs::Point origin_position;
    bool state_has_changed;

public:
    decision_node()
    {

        // communication with datmo_node
        sub_person_position = n.subscribe("person_position", 1, &decision_node::person_positionCallback, this);

        pub_datmo_marker = n.advertise<visualization_msgs::Marker>("decision_marker", 1);

        // communication with rotation_node
        pub_rotation_to_do = n.advertise<geometry_msgs::Point>("goal_to_rotate", 1);

        // communication with action_node
        pub_goal_to_reach = n.advertise<geometry_msgs::Point>("goal_to_reach", 1); // Preparing a topic to publish the position of the person

        // communication with robot_moving_node
        sub_robot_moving = n.subscribe("robot_moving", 1, &decision_node::robot_movingCallback, this);

        // communication with localization node
        sub_localization = n.subscribe("localization", 1, &decision_node::localizationCallback, this);

        current_state = waiting_for_a_person;
        previous_state = -1;

        state_has_changed = false;

        // TO DEFINE according to the position of the base/initial position in the map
        base_position.x = 0;
        base_position.y = 0;
        base_orientation = 0;

        origin_position.x = 0;
        origin_position.y = 0;

        person_tracked = false;
        person_moving = false;
        init_localization = false;

        // INFINITE LOOP TO COLLECT LASER DATA AND PROCESS THEM
        ros::Rate r(10); // this node will work at 10hz
        while (ros::ok())
        {
            ros::spinOnce(); // each callback is called once
            update();
            r.sleep(); // we wait if the processing (ie, callback+update) has taken less than 0.1s (ie, 10 hz)
        }
    }

    // UPDATE: main processing of laser data
    /*//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/
    void update()
    {

        if (init_localization)
        {

            update_variables();

            switch (current_state)
            {
            case waiting_for_a_person:
                process_waiting_for_a_person();
                break;

            case observing_the_person:
                process_observing_the_person();
                break;

            case rotating_to_the_person:
                process_rotating_to_the_person();
                break;

            case moving_to_the_person:
                process_moving_to_the_person();
                break;

            case interacting_with_the_person:
                process_interacting_with_the_person();
                break;

            case rotating_to_the_base:
                process_rotating_to_the_base();
                break;

            case moving_to_the_base:
                process_moving_to_the_base();
                break;

            case resetting_orientation:
                process_resetting_orientation();
                break;
            }

        if (translation_to_base > max_base_distance)
            current_state = rotating_to_the_base;

        new_localization = false;

        state_has_changed = current_state != previous_state;
        previous_state = current_state;

        ROS_WARN("SELF POINT: %f, %f, %f", current_position.x, current_position.y, current_position.z);
        ROS_WARN("BASE POINT: %f, %f, %f", local_base_position.x, local_base_position.y, local_base_position.z);
        populateMarkerTopic();
    }
    else ROS_WARN("Initialize localization");

} // update

void
update_variables()
{
    if (new_person_position)
    {
        translation_to_person = distancePoints(origin_position, person_position);

        if (translation_to_person > 0)
        {
            rotation_to_person = acos(person_position.x / translation_to_person);
            if (person_position.y < 0)
                rotation_to_person *= -1;
        }
        else
            rotation_to_person = 0;

        person_tracked = person_position.x != 0 || person_position.y != 0;
        person_moving = distancePoints(person_position, last_person) >= person_moving_epsilon;

        last_person.x = person_position.x;
        last_person.y = person_position.y;
    }

    if (new_localization)
    {
        // when we receive a new position(x, y, o) of robair in the map, we update:
        // local_base_position: the position of the base in the cartesian local frame of robot
        local_base_position.x = current_position.x - base_position.x;
        local_base_position.y = current_position.y - base_position.y;
        // translation_to_base: the translation that robair has to do to reach its base
        translation_to_base = distancePoints(current_position, base_position);
        // rotation_to_base: the rotation that robair has to do to reach its base
        rotation_to_base = - current_orientation + base_orientation;
    }
}

void process_waiting_for_a_person()
{

    if (state_has_changed)
        ROS_WARN("current_state: waiting_for_a_person");

    // Processing of the state
    // as soon as we detect a moving person, we switch to the state "observing_the_person"
    if (new_person_position && person_tracked)
        current_state = observing_the_person;
}

void process_observing_the_person()
{

    if (state_has_changed)
    {
        ROS_WARN("current_state: observing_the_person");
        ROS_WARN("person_position: (%f, %f)", person_position.x, person_position.y);
        frequency = 0;
    }

    // Processing of the state
    // Robair only observes and tracks the moving person
    // if the moving person does not move during a while (use frequency), we switch to the state "rotating_to_the_person"
    if (new_person_position && person_moving)
    {
        frequency = 0;
    }
    else
        frequency++;

    ROS_WARN("observing_the_person: (%f, %f), %d", person_position.x, person_position.y, frequency);
    if (frequency >= frequency_expected)
        current_state = rotating_to_the_person;
}

void process_rotating_to_the_person()
{

    if (state_has_changed)
    {
        ROS_WARN("current_state: rotating_to_the_person");
        ROS_WARN("person_position: (%f, %f)", person_position.x, person_position.y);
        frequency = 0;
    }

    // Processing of the state
    // Robair rotates to be face to the moving person
    // if robair is face to the moving person and the moving person does not move during a while (use frequency), we switch to the state "moving_to_the_person"
    if (person_moving)
    {
        ROS_WARN("person_position: (%f, %f)", person_position.x, person_position.y);
        frequency = 0;
    }
    else
        frequency++;

    ROS_WARN("rotating_to_the_person: (%f, %f), %f, %d", person_position.x, person_position.y, rotation_to_person, frequency);
    pub_rotation_to_do.publish(person_position);
    if (!person_tracked)
        current_state = resetting_orientation;
    else if (frequency >= frequency_expected && abs(rotation_to_person) < rotation_epsilon)
        current_state = moving_to_the_person;
}

void process_moving_to_the_person()
{

    if (state_has_changed)
    {
        ROS_WARN("current_state: moving_to_the_person");
        ROS_WARN("person_position: (%f, %f)", person_position.x, person_position.y);
        frequency = 0;
    }

    // Processing of the state
    // Robair moves to be close to the moving person
    // if robair is close to the moving person and the moving person does not move during a while (use frequency), we switch to the state "interacting_with_the_person"
    if (person_moving)
    {
        ROS_WARN("person_position: (%f, %f)", person_position.x, person_position.y);
        frequency = 0;
    }
    else
        frequency++;

    ROS_WARN("moving_to_the_person: (%f, %f), %d", person_position.x, person_position.y, frequency);
    pub_goal_to_reach.publish(person_position);
    if (translation_to_base > max_base_distance || !person_tracked)
        current_state = rotating_to_the_base;
    else if (frequency >= frequency_expected && translation_to_person < approach_epsilon)
        current_state = interacting_with_the_person;
}

void process_interacting_with_the_person()
{

    if (state_has_changed)
    {
        ROS_WARN("current_state: interacting_with_the_person");
        ROS_WARN("person_position: (%f, %f)", person_position.x, person_position.y);
        frequency = 0;
    }

    // Processing of the state
    // Robair does not move and interacts with the moving person until the moving person goes away from robair
    // if the person goes away from robair, after a while (use frequency), we switch to the state "rotating_to_the_base"
    if (new_person_position)
    {
        ROS_WARN("person_position: (%f, %f)", person_position.x, person_position.y);
    }

    // To make the robot wait for a while. Safeguards the radar malfunctioning.
    if (!person_tracked)
        frequency++;
    else 
        frequency = 0;

    if (translation_to_person > interaction_epsilon || (!person_tracked && frequency >= frequency_expected))
        current_state = rotating_to_the_base;
}

void process_rotating_to_the_base()
{

    if (state_has_changed)
    {
        ROS_WARN("current_state: rotating_to_the_base");
        ROS_WARN("position of robair in the map: (%f, %f, %f)", current_position.x, current_position.y, current_orientation * 180 / M_PI);
    }

    // Processing of the state
    // Robair rotates to be face to its base
    // if robair is face to its base and does not move, after a while (use frequency), we switch to the state "moving_to_the_base"
    if (new_localization)
    {
        ROS_WARN("position of robair in the map: (%f, %f, %f)", current_position.x, current_position.y, current_orientation * 180 / M_PI);
        ROS_WARN("Local Base Position: (%lf, %lf, %lf)", local_base_position.x, local_base_position.y, local_base_position.z);
    }

    local_base_position.z = rotation_to_base;
    pub_rotation_to_do.publish(local_base_position);
    if (rotation_to_base < rotation_epsilon)
        current_state = moving_to_the_base;
}

void process_moving_to_the_base()
{

    if (state_has_changed)
    {
        ROS_WARN("current_state: moving_to_the_base");
        ROS_WARN("position of robair in the map: (%f, %f, %f)", current_position.x, current_position.y, current_orientation * 180 / M_PI);
    }

    // Processing of the state
    // Robair moves to its base
    // if robair is close to its base and does not move, after a while (use frequency), we switch to the state "resetting_orientation"
    if (new_localization)
    {
        ROS_WARN("position of robair in the map: (%f, %f, %f)", current_position.x, current_position.y, current_orientation * 180 / M_PI);
        ROS_WARN("Local Base Position: (%lf, %lf, %lf)", local_base_position.x, local_base_position.y, local_base_position.z);
    }

    pub_goal_to_reach.publish(local_base_position);
    if (translation_to_base < translation_epsilon)
        current_state = resetting_orientation;
}

void process_resetting_orientation()
{

    if (state_has_changed)
    {
        ROS_WARN("current_state: initializing_rotation");
        ROS_WARN("position of robair in the map: (%f, %f, %f)", current_position.x, current_position.y, current_orientation * 180 / M_PI);
        frequency = 0;
    }

    // Processing of the state
    // Robair rotates to its initial orientation
    // if robair is close to its initial orientation and does not move, after a while (use frequency), we switch to the state "waiting_for_a_person"
    if (new_localization)
    {
        ROS_WARN("position of robair in the map: (%f, %f, %f)", current_position.x, current_position.y, current_orientation * 180 / M_PI);
        frequency = 0;
    }
    else
        frequency++;

    float difference = base_orientation - current_orientation;
    local_base_position.z = difference;
    pub_rotation_to_do.publish(local_base_position);
    if (frequency >= frequency_expected && difference < rotation_epsilon)
        current_state = waiting_for_a_person;
}

// CALLBACKS
/*//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/
void person_positionCallback(const geometry_msgs::Point::ConstPtr &g)
{
    // process the goal received from moving_persons detector

    new_person_position = true;
    person_position.x = g->x;
    person_position.y = g->y;
    person_position.z = g->z;
    ROS_WARN("Person position: (%lf, %lf, %lf)", g->x, g->y, g->z);
}

void robot_movingCallback(const std_msgs::Bool::ConstPtr &state)
{

    robot_moving = state->data;

} // robot_movingCallback

void localizationCallback(const geometry_msgs::Point::ConstPtr &l)
{
    // process the localization received from my localization

    if (abs(l->x) + abs(l->y) + abs(l->z) < float_comparison_precision) return;

    if (!init_localization)
    {
        base_position = *l;
        base_orientation = l->z;
    }

    new_localization = true;
    init_localization = true;
    current_position = *l;
    current_orientation = l->z;
}

void populateMarkerTopic()
    {

        visualization_msgs::Marker marker;

        marker.header.frame_id = "laser";
        marker.header.stamp = ros::Time::now();
        marker.ns = "base_detector_tracker";
        marker.id = 0;
        marker.type = visualization_msgs::Marker::POINTS;
        marker.action = visualization_msgs::Marker::ADD;

        marker.pose.orientation.w = 1;

        marker.scale.x = 0.1;
        marker.scale.y = 0.1;

        marker.color.a = 1.0;

        // ROS_INFO("%i points to display", nb_pts);
        geometry_msgs::Point p;
        std_msgs::ColorRGBA c;

        p.x = base_position.x;
        p.y = base_position.y;
        p.z = base_position.z;

        c.r = 1;
        c.g = 0;
        c.b = 0;
        c.a = 1;

        // ROS_INFO("(%f, %f, %f) with rgba (%f, %f, %f, %f)", p.x, p.y, p.z, c.r, c.g, c.b, c.a);
        marker.points.push_back(p);
        marker.colors.push_back(c);

        pub_datmo_marker.publish(marker);
    }

// Distance between two points
float distancePoints(geometry_msgs::Point pa, geometry_msgs::Point pb)
{

    return sqrt(pow((pa.x - pb.x), 2.0) + pow((pa.y - pb.y), 2.0));
}
}
;

int main(int argc, char **argv)
{

    ROS_INFO("(decision_node) waiting for a /person_position");
    ros::init(argc, argv, "decision_node");

    decision_node bsObject;

    ros::spin();

    return 0;
}
