from typing import Any, Callable

from rospy import init_node, get_param

from .color import color


def _set_attribute(name: str) -> Callable[["Params", Any], None]:
    def set(self, value: Any) -> None:
        setattr(self, name, value)
    return set


def _get_attribute(name: str) -> Callable[["Params"], Any]:
    def get(self: "Params") -> Any:
        try:
            return getattr(self, name)
        except AttributeError:
            raise AttributeError(f"Parameter '{name}' wasn't initialized!!")
    return get


def _property(name: str):
    return property(_get_attribute(name), _set_attribute(name))


class Params:
    # Node names
    LIVE_LOADER = "follow_the_drow_live_loader"
    FILE_LOADER = "follow_the_drow_file_loader"
    VISUALIZER = "follow_the_drow_visualizer"
    ALGORITHMIC_DETECTOR = "follow_the_drow_algorithmic_detector"
    DROW_DETECTOR = "follow_the_drow_drow_detector"
    DATA_ANNOTATOR = "follow_the_drow_data_annotator"
    PERSON_TRACKER = "follow_the_drow_person_tracker"

    # General arguments
    HEARTBEAT_RATE = _property("HEARTBEAT_RATE")
    _HEARTBEAT_RATE_NAME = "heartbeat_rate"
    RAW_DATA_TOPIC = _property("RAW_DATA_TOPIC")
    _RAW_DATA_TOPIC_NAME = "raw_data_topic"
    ANNOTATED_DATA_TOPIC = _property("ANNOTATED_DATA_TOPIC")
    _ANNOTATED_DATA_TOPIC_NAME = "annotated_data_topic"
    ALGORITHMIC_DETECTOR_TOPIC = _property("ALGORITHMIC_DETECTOR_TOPIC")
    _ALGORITHMIC_DETECTOR_TOPIC_NAME = "algorithmic_detector_topic"
    DROW_DETECTOR_TOPIC = _property("DROW_DETECTOR_TOPIC")
    _DROW_DETECTOR_TOPIC_NAME = "drow_detector_topic"
    DATA_ANNOTATION_RATE = _property("DATA_ANNOTATION_RATE")
    _DATA_ANNOTATION_RATE_NAME = "data_annotation_rate"
    GENERAL_FRAME = _property("GENERAL_FRAME")
    _GENERAL_FRAME_NAME = "general_frame"

    # File loader node arguments
    FILE_LOADER_DATASET_PATH = _property("FILE_LOADER_DATASET_PATH")
    _FILE_LOADER_DATASET_PATH_NAME = "dataset_path"
    FILE_LOADER_VERBOSE = _property("FILE_LOADER_VERBOSE")
    _FILE_LOADER_VERBOSE_NAME = "verbose"
    FILE_LOADER_PERSONS_ONLY = _property("FILE_LOADER_PERSONS_ONLY")
    _FILE_LOADER_PERSONS_ONLY_NAME = "persons_only"

    # DROW detector node arguments
    DROW_DETECTOR_PERSONS_ONLY = _property("DROW_DETECTOR_PERSONS_ONLY")
    _DROW_DETECTOR_PERSONS_ONLY_NAME = "persons_only"
    DROW_DETECTOR_TRACKING_POLICY = _property("DROW_DETECTOR_TRACKING_POLICY")
    _DROW_DETECTOR_TRACKING_POLICY_NAME = "tracking_policy"
    DROW_DETECTOR_THRESHOLD = _property("DROW_DETECTOR_THRESHOLD")
    _DROW_DETECTOR_THRESHOLD_NAME = "threshold"
    DROW_DETECTOR_VERBOSE = _property("DROW_DETECTOR_VERBOSE")
    _DROW_DETECTOR_VERBOSE_NAME = "verbose"

    # Data annotator
    DATA_ANNOTATOR_DATASET_PATH = _property("DATA_ANNOTATOR_DATASET_PATH")
    _DATA_ANNOTATOR_DATASET_PATH_NAME = "dataset_path"
    BACKGROUND_COLOR = _property("BACKGROUND_COLOR")
    _BACKGROUND_COLOR_NAME = "backgorund_color"
    ANNOTATED_COLOR = _property("ANNOTATED_COLOR")
    _ANNOTATED_COLOR_NAME = "annotated_color"
    VISUALIZATION_TOPIC = _property("VISUALIZATION_TOPIC")
    _VISUALIZATION_TOPIC_NAME = "visualization_topic"
    ANNOTATION_FILE = _property("ANNOTATION_FILE")
    _ANNOTATION_FILE_NAME = "output_file"
    DATA_ANNOTATOR_VERBOSE = _property("DATA_ANNOTATOR_VERBOSE")
    _DATA_ANNOTATOR_VERBOSE_NAME = "verbose"


def load_args_for_node(name: str):
    init_node(name)
    Params.HEARTBEAT_RATE = get_param(f"/{Params._HEARTBEAT_RATE_NAME}")
    Params.RAW_DATA_TOPIC = get_param(f"/{Params._RAW_DATA_TOPIC_NAME}")
    Params.ANNOTATED_DATA_TOPIC = get_param(f"/{Params._ANNOTATED_DATA_TOPIC_NAME}")
    Params.ALGORITHMIC_DETECTOR_TOPIC = get_param(f"/{Params._ALGORITHMIC_DETECTOR_TOPIC_NAME}")
    Params.DROW_DETECTOR_TOPIC = get_param(f"/{Params._DROW_DETECTOR_TOPIC_NAME}")
    Params.DATA_ANNOTATION_RATE = get_param(f"/{Params._DATA_ANNOTATION_RATE_NAME}")
    Params.GENERAL_FRAME = get_param(f"/{Params._GENERAL_FRAME_NAME}")

    if name == Params.FILE_LOADER:
        Params.FILE_LOADER_DATASET_PATH = get_param(f"/{name}/{Params._FILE_LOADER_DATASET_PATH_NAME}")
        Params.FILE_LOADER_VERBOSE = get_param(f"/{name}/{Params._FILE_LOADER_VERBOSE_NAME}")
        Params.FILE_LOADER_PERSONS_ONLY = get_param(f"/{name}/{Params._FILE_LOADER_PERSONS_ONLY_NAME}")
    elif name == Params.DROW_DETECTOR:
        Params.DROW_DETECTOR_PERSONS_ONLY = get_param(f"/{name}/{Params._DROW_DETECTOR_PERSONS_ONLY_NAME}")
        Params.DROW_DETECTOR_THRESHOLD = get_param(f"/{name}/{Params._DROW_DETECTOR_THRESHOLD_NAME}")
        Params.DROW_DETECTOR_VERBOSE = get_param(f"/{name}/{Params._DROW_DETECTOR_VERBOSE_NAME}")
    elif name == Params.DATA_ANNOTATOR:
        Params.DATA_ANNOTATOR_DATASET_PATH = get_param(f"/{name}/{Params._DATA_ANNOTATOR_DATASET_PATH_NAME}")
        Params.BACKGROUND_COLOR = color(get_param(f"/{name}/{Params._BACKGROUND_COLOR_NAME}"))
        Params.ANNOTATED_COLOR = color(get_param(f"/{name}/{Params._ANNOTATED_COLOR_NAME}"))
        Params.VISUALIZATION_TOPIC = get_param(f"/{name}/{Params._VISUALIZATION_TOPIC_NAME}")
        Params.ANNOTATION_FILE = get_param(f"/{name}/{Params._ANNOTATION_FILE_NAME}")
        Params.DATA_ANNOTATOR_VERBOSE = get_param(f"/{name}/{Params._DATA_ANNOTATOR_VERBOSE_NAME}")
    else:
        raise RuntimeError(f"Unknown node name '{name}'!")
