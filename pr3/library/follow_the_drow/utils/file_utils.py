from contextlib import contextmanager
from os import makedirs
from pathlib import Path
from pickle import dumps, loads
from typing import Any, Callable, Iterator, Tuple, Union

_ReturnType = Union[Tuple[Any, ...], Any]

DROW_DATA_PATH = Path("include/DROWv2-data")
DROW_TRAIN_SET = Path("train")
DROW_VALIDATION_SET = Path("val")
DROW_TEST_SET = Path("test")
DROW_WEIGHTS_PATH = Path("include/weights.pth.tar")

CACHE_DIR = Path.cwd() / Path("cache")


@contextmanager
def file_cache_function(file: Union[Path, str], function: Callable[..., _ReturnType], *arguments: Any, path: Path = CACHE_DIR, force: bool = False) -> Iterator[Tuple[_ReturnType, bool]]:
    cache_file = path / Path(file)
    makedirs(cache_file.parent, exist_ok=True)
    if not cache_file.exists() or force:
        result = function(*arguments)
        cache_file.write_bytes(dumps(result))
        yield result, True
    else:
        yield loads(cache_file.read_bytes()), False
