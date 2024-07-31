#!/usr/bin python3

from numpy import array, zeros, float32, argmax, any

from rospy import Publisher, Subscriber, Rate, is_shutdown, spin
from geometry_msgs.msg import Point

from follow_the_drow import Params, load_args_for_node
from follow_the_drow.msg import raw_data, detection
from follow_the_drow.detectors import DrowDetector
from follow_the_drow.datasets import LiveDataset
from follow_the_drow.utils.drow_utils import cutout, prepare_prec_rec_softmax, votes_to_detections


class DROWDetector:
    RESULT_CONF = {
        "blur_sigma": 2.23409276092903,
        "blur_win": 11,
        "bin_size": 0.04566379529562327,
        "vote_collect_radius": 0.6351825665330302,
        "min_thresh": 0.0027015322261551397,
        "class_weights": [0.89740097838073, 0.3280190481521334, 0.4575675717820713]
    }

    def __init__(self, persons_only: bool, threshold: float, verbose: bool) -> None:
        self.persons_only = persons_only
        self.threshold = threshold
        self.dataset = LiveDataset(time_frame_size=Params.DATA_ANNOTATION_RATE, verbose=verbose)
        self.detector = DrowDetector.init(time_frame_size=self.dataset.time_frame, verbose=verbose)
        self.drow_data = Publisher(Params.DROW_DETECTOR_TOPIC, detection, queue_size=1)
        self.raw_data = Subscriber(Params.RAW_DATA_TOPIC, raw_data, self.callback, queue_size=10)
        self.rate = Rate(Params.HEARTBEAT_RATE)
        self.data_initialized = False
        self.previous = None

    def callback(self, raw_data):
        bottom_lidar = array([measure.x for measure in raw_data.bottom_lidar], dtype=float32)
        top_lidar = array([measure.x for measure in raw_data.top_lidar], dtype=float32)
        odometry = zeros(1, dtype=[("xya", float32, 3)])
        odometry[0]["xya"] = (raw_data.odometry.x, raw_data.odometry.y, raw_data.odometry.z)
        self.dataset.push_measure(bottom_lidar, top_lidar, odometry[0])
        self.data_initialized = True

    def update(self):
        if self.data_initialized:
            scans, odoms = self.dataset.get_bottom_scan()
            cut = cutout(scans, odoms, scans.shape[-1], nsamp=self.detector.N_SAMP)
            confs, offs = self.detector.forward_one(cut)
            x, y = prepare_prec_rec_softmax(scans, array([offs]))
            if len(x) == 0 and len(y) == 0:
                detections = list()
            else:
                detections = votes_to_detections(x, y, array([confs]), **self.RESULT_CONF)[-1]
            if self.persons_only:
                detections = [det for det in detections if argmax(det[2][1:]) == 2 and det[2][3] > self.threshold]
            else:
                detections = [det for det in detections if any(det[2][1:] > self.threshold)]
            # WARNING! Here, with points 'x' and 'y' are swapped!
            points = [Point(x=detect[1], y=detect[0]) for detect in detections]
            self.drow_data.publish(detection(detection=points))

    def __call__(self) -> None:
        while not is_shutdown():
            self.update()
            self.rate.sleep()


if __name__ == "__main__":
    load_args_for_node(Params.DROW_DETECTOR)
    DROWDetector(Params.DROW_DETECTOR_PERSONS_ONLY, Params.DROW_DETECTOR_THRESHOLD, Params.DROW_DETECTOR_VERBOSE).__call__()
    spin()
