import pandas as pd
import json


def collect_list(dictionary: list) -> pd.DataFrame:
    return pd.DataFrame(dictionary)


def collect_dict(dictionary: dict) -> pd.DataFrame:
    return pd.DataFrame(dictionary)


def collect_json(json_string: str) -> pd.DataFrame:
    return pd.DataFrame(json.loads(json_string))


def collect_json_file(json_file: str) -> pd.DataFrame:
    with open(f"{json_file}.json", 'r') as file:
        return pd.DataFrame(json.loads(file.read()))


def collect_csv(csv_string: str) -> pd.DataFrame:
    pass


def collect_csv_file(csv_file: str) -> pd.DataFrame:
    pass
