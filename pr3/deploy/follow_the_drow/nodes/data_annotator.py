#!/usr/bin python3

from os import getenv
from pathlib import Path
from typing import Optional, List, Tuple

from rospy import Publisher, Subscriber, Time, Header, spin, loginfo
from std_msgs.msg import ColorRGBA
from geometry_msgs.msg import Point, PointStamped, PoseStamped, Pose, Vector3, Quaternion, PoseWithCovarianceStamped
from visualization_msgs.msg import Marker

from follow_the_drow import Params, load_args_for_node
from follow_the_drow.datasets import DROW_Dataset
from follow_the_drow.utils.drow_utils import laser_angles, rphi_to_xy

_DETECTION_TOPIC = "/clicked_point"
_FORWARD_TOPIC = "/initialpose"
_CLEAR_TOPIC = "/move_base_simple/goal"


class Frame:
    def __init__(self, dataset: DROW_Dataset, file_index: int = 0, current_scan: int = 0):
        self.dataset = dataset
        self.file_index = file_index
        self.current = current_scan

    def _copy(self, dataset: Optional[DROW_Dataset] = None, file_index: Optional[int] = None, current_scan: Optional[int] = None):
        dataset = self.dataset if dataset is None else dataset
        file_index = self.file_index if file_index is None else file_index
        current_scan = self.current_scan if current_scan is None else current_scan
        return Frame(dataset, file_index, current_scan)

    @property
    def filename(self) -> str:
        return self.dataset.filenames[self.file_index].split("/")[-1]
    
    @property
    def empty_annotation(self):
        return self.dataset.det_id[self.file_index][self.current], list()

    @property
    def forward(self) -> Tuple[Optional["Frame"], bool]:
        if self.current == len(self.dataset.det_id[self.file_index]) - 1:
            if self.file_index == len(self.dataset.det_id) - 1:
                return None, True
            else:
                return self._copy(file_index=self.file_index + 1, current_scan=0), True
        else:
            return self._copy(current_scan=self.current + 1), False

    @property
    def backward(self) -> Tuple[Optional["Frame"], bool]:
        if self.current == 0:
            if self.file_index == 0:
                return None, True
            else:
                max_scan_index = len(self.dataset.det_id[self.file_index]) - 1
                return self._copy(file_index=self.file_index - 1, current_scan=max_scan_index), True
        else:
            return self._copy(current_scan=self.current - 1), False

    @property
    def data(self) -> List:
        scan = self.dataset.idet2iscan[self.file_index][self.current]
        data = self.dataset.scans[self.file_index][scan]
        angles = laser_angles(data.shape[-1])[None, :]
        xs, ys = rphi_to_xy(data, angles)
        return [Point(x=x, y=y) for x, y in zip(xs[0], ys[0])]


class DataAnnotator:
    def __init__(self, dataset: Path, background_color: ColorRGBA, annotated_color: ColorRGBA, visualization_topic: str, annotation_file: str, verbose: bool) -> None:
        self.output_path = Path(getenv("OUTPUT_PATH"))
        self.background_color = background_color
        self.annotated_color = annotated_color
        self.visualization_topic = visualization_topic
        self.annotation_file = annotation_file
        self.dataset = DROW_Dataset(dataset=dataset, time_frame_size=Params.DATA_ANNOTATION_RATE, verbose=verbose)
        self.annotated = Publisher(visualization_topic, Marker, queue_size=10)
        self.detection = Subscriber(_DETECTION_TOPIC, PointStamped, self.detection_callback, queue_size=10)
        self.clear = Subscriber(_CLEAR_TOPIC, PoseStamped, self.clear_callback, queue_size=10)
        self.forward = Subscriber(_FORWARD_TOPIC, PoseWithCovarianceStamped, self.forward_callback, queue_size=10)
        self.frame = Frame(self.dataset)
        self.annotations = list()
        if verbose:
            loginfo(f"Dataset includes {len(self.dataset.scan_id)} scans and {len(self.dataset.det_id)} detections")
        loginfo(f"\n\nMark all people with points on current frame:\n\tPress 'backspace' to clear current frame annotations.\n\tPress 'backspace' second time to go to the previous frame.\n\tPress 'enter' to advance to the next frame.")

    def detection_callback(self, point: PointStamped):
        self.annotations[self.frame.current][1].append(point.point)
        self.update()

    def clear_callback(self, _: PoseStamped):
        if len(self.annotations[self.frame.current][1]) > 0:
            self.annotations[self.frame.current] = self.frame.empty_annotation
        else:
            self.move_frame(*self.frame.backward)
        self.update()

    def forward_callback(self, _: PoseWithCovarianceStamped):
        self.move_frame(*self.frame.forward)
        self.update()

    def move_frame(self, new_frame, update):
        if update:
            self.dump_annotations(self.frame.filename)
            self.annotations = [self.frame.empty_annotation]
        if new_frame is not None:
            self.frame = new_frame
            self.annotations.append(self.frame.empty_annotation)

    def dump_annotations(self, filename: str):
        output = str()
        for id, annotation in self.annotations:
            points = [f"[{x},{y}]" for x, y in annotation]
            output += f"{id},[{','.join(points)}]\n"
        if self.output_path is not None:
            name = self.output_path / self.annotation_file.replace("*", filename)
            with open(name, "w") as file:
                file.write(output)
        else:
            print(output)

    def update(self):
        header = Header(frame_id=Params.GENERAL_FRAME, stamp=Time.now())
        pose = Pose(orientation=Quaternion(w=1))
        scale = Vector3(x=0.05, y=0.05)
        color = ColorRGBA(a=1.0)

        annotated_data = self.annotations[self.frame.current][1]
        marker = Marker(header=header, pose=pose, scale=scale, color=color, ns=self.visualization_topic, id=0, type=Marker.POINTS, action=Marker.ADD)

        for point in self.frame.data:
            marker.points.append(point)
            marker.colors.append(self.background_color)
        for point in annotated_data:
            marker.points.append(point)
            marker.colors.append(self.annotated_color)
        
        self.annotated.publish(marker)

    def __call__(self) -> None:
        self.current_frame = Frame(self.dataset)
        self.annotations = [self.frame.empty_annotation]
        self.update()


if __name__ == "__main__":
    load_args_for_node(Params.DATA_ANNOTATOR)
    DataAnnotator(Params.DATA_ANNOTATOR_DATASET_PATH, Params.BACKGROUND_COLOR, Params.ANNOTATED_COLOR, Params.VISUALIZATION_TOPIC, Params.ANNOTATION_FILE, Params.DATA_ANNOTATOR_VERBOSE).__call__()
    spin()
