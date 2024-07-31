from configparser import ConfigParser
from typing import Dict, Union


def _number(numeric: str) -> Union[int, float]:
    return float(numeric) if "." in numeric else int(numeric)


def read_config(filename: str = "default.ini") -> Dict[str, Dict[str, Union[int, float]]]:
    config = ConfigParser()
    config.read(filename)

    data = dict()
    for section in config.sections():
        data[section] = {key: _number(config[section][key]) for key in config[section]}
    return data
