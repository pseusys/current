# laser text display

from typing import List, Optional
from math import pi, sin, cos
from pathlib import Path

from rospy import loginfo, Subscriber, Rate, is_shutdown, init_node, spin
from sensor_msgs.msg import LaserScan
from geometry_msgs.msg import Point


class LaserTextDisplayNode:
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

        # to check if new data of laser is available or not
        self.new_laser = False

    def callback(self, scan: LaserScan):
        self.new_laser = True

        # store the important data related to laserscanner
        self.range_min = scan.range_min
        self.range_max = scan.range_max
        self.angle_min = scan.angle_min
        self.angle_max = scan.angle_max
        self.angle_inc = scan.angle_increment
        self.nb_beams = int(((-1 * scan.angle_min) + scan.angle_max) / scan.angle_increment)

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
        loginfo("New data of laser received")
        for loop in range(self.nb_beams):
            loginfo(
                f"r[{loop}] = {self.r[loop]:.3f}, "
                f"theta[{loop}] (in degrees) = {self.theta[loop] * 180 / pi:.3f}, "
                f"x[{loop}] = {self.current_scan[loop].x:.3f}, "
                f"y[{loop}] = {self.current_scan[loop].y:.3f}"
            )

    def __call__(self, subscriber_name="scan"):
        Subscriber(subscriber_name, LaserScan, self.callback, queue_size=1)

        # INFINITE LOOP TO COLLECT LASER DATA AND PROCESS THEM
        rate = Rate(10)  # this node will run at 10hz
        while not is_shutdown():
            if self.new_laser:
                self.update()  # processing of data
            rate.sleep()  # we wait if the processing (ie, callback+update) has taken less than 0.1s (ie, 10 hz)


if __name__ == '__main__':
    init_node(Path(__file__).stem)
    LaserTextDisplayNode().__call__()
    spin()
