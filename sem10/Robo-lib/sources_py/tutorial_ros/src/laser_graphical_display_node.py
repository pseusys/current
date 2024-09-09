# laser text display

from typing import List, Optional
from math import sin, cos
from pathlib import Path

from rospy import loginfo, Subscriber, Rate, is_shutdown, init_node, spin, Publisher, Time
from std_msgs.msg import ColorRGBA
from sensor_msgs.msg import LaserScan
from geometry_msgs.msg import Point
from visualization_msgs.msg import Marker


class LaserGraphicalDisplayNode:
    def __init__(self):
        # to store, process and display laserdata
        self.nb_beams = -1
        self.range_min = -1.0
        self.range_max = -1.0
        self.angle_min = -1.0
        self.angle_max = -1.0
        self.angle_inc = -1.0
        self.r = [-1.0] * 1000
        self.theta = [-1.0] * 1000
        self.current_scan: List[Optional[Point]] = [None] * 1000

        self.pub_laser_graphical_display_marker: Optional[Publisher] = None

        # to check if new data of laser is available or not
        self.new_laser = False

        # GRAPHICAL DISPLAY
        self.nb_pts = -1
        self.display: List[Optional[Point]] = [None] * 1000
        self.colors: List[Optional[ColorRGBA]] = [None] * 1000

    def callback(self, scan: LaserScan):
        self.new_laser = True

        # store the important data related to laserscanner
        self.range_min = scan.range_min
        self.range_max = scan.range_max
        self.angle_min = scan.angle_min
        self.angle_max = scan.angle_max
        self.angle_inc = scan.angle_increment
        self.nb_beams = self.nb_pts = int(((-1 * scan.angle_min) + scan.angle_max) / scan.angle_increment)

        # store the range and the coordinates in cartesian framework of each hit
        beam_angle = self.angle_min
        for loop in range(self.nb_beams):
            beam_angle += self.angle_inc

            if self.range_max > scan.ranges[loop] > self.range_min:
                self.r[loop] = scan.ranges[loop]
            else:
                self.r[loop] = self.range_max
            self.theta[loop] = beam_angle

            # transform the scan in cartesian framework
            self.current_scan[loop] = Point(x=self.r[loop] * cos(beam_angle), y=self.r[loop] * sin(beam_angle), z=0.0)

    def update(self):
        # UPDATE: main processing of laser data and robot_moving
        self.new_laser = False
        for loop in range(self.nb_beams):
            self.display[loop] = self.current_scan[loop]
            self.colors[loop] = ColorRGBA(r=0, g=0, b=1, a=1.0)

        self.populate_marker_topic()

    def populate_marker_topic(self):
        # Draw the field of view and other references
        marker = Marker()

        marker.header.frame_id = "laser"
        marker.header.stamp = Time.now()
        marker.ns = "laser_graphical_display"
        marker.id = 0
        marker.type = Marker.POINTS
        marker.action = Marker.ADD

        marker.pose.orientation.w = 1

        marker.scale.x = 0.05
        marker.scale.y = 0.05

        marker.color.a = 1.0

        for loop in range(self.nb_pts):
            p = Point(x=self.display[loop].x, y=self.display[loop].y, z=self.display[loop].z)
            c = ColorRGBA(r=self.colors[loop].r, g=self.colors[loop].g, b=self.colors[loop].b, a=self.colors[loop].a)

            marker.points.append(p)
            marker.colors.append(c)

        self.pub_laser_graphical_display_marker.publish(marker)
        self.populate_marker_reference()

    def populate_marker_reference(self):
        references = Marker()

        references.header.frame_id = "laser"
        references.header.stamp = Time.now()
        references.ns = "laser_graphical_display"
        references.id = 1
        references.type = Marker.LINE_STRIP
        references.action = Marker.ADD
        references.pose.orientation.w = 1

        references.scale.x = 0.02
        references.color = ColorRGBA(r=1.0, g=1.0, b=1.0, a=1.0)

        references.points.append(Point(x=0.02 * cos(-2.356194), y=0.02 * sin(-2.356194), z=0.0))
        references.points.append(Point(x=5.6 * cos(-2.356194), y=5.6 * sin(-2.356194), z=0.0))

        beam_angle = -2.356194 + 0.006136
        for loop in range(723):
            beam_angle += 0.006136
            references.points.append(Point(x=5.6 * cos(beam_angle), y=5.6 * sin(beam_angle), z=0.0))

        references.points.append(Point(x=5.6 * cos(2.092350), y=5.6 * sin(2.092350), z=0.0))
        references.points.append(Point(x=0.02 * cos(2.092350), y=0.02 * sin(2.092350), z=0.0))

        self.pub_laser_graphical_display_marker.publish(references)

    def __call__(self, subscriber_name="scan", advertiser_name="laser_graphical_display_marker"):
        Subscriber(subscriber_name, LaserScan, self.callback, queue_size=1)
        # Preparing a topic to publish our results. This will be used by the visualization tool rviz
        self.pub_laser_graphical_display_marker = Publisher(advertiser_name, Marker, queue_size=1)

        # INFINITE LOOP TO COLLECT LASER DATA AND PROCESS THEM
        rate = Rate(10)  # this node will run at 10hz
        while not is_shutdown():
            if self.new_laser:
                self.update()  # processing of data
            rate.sleep()  # we wait if the processing (ie, callback+update) has taken less than 0.1s (ie, 10 hz)


if __name__ == '__main__':
    init_node(Path(__file__).stem)
    LaserGraphicalDisplayNode().__call__()
    spin()
