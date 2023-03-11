from pathlib import Path
from sys import exit
from logging import getLogger, StreamHandler, FileHandler
from typing import Union, Optional

_COLOR_RESET = "\u001B[0m"
_COLOR_RED = "\u001B[31m"
_COLOR_GREEN = "\u001B[32m"
_COLOR_BLUE = "\u001B[34m"
_COLOR_YELLOW = "\u001B[33m"

_logger = getLogger(__name__)
_logger.addHandler(StreamHandler())


def setup_logger(level: str, file: Optional[str] = None):
    _logger.setLevel(level)
    if file is not None:
        Path(file).parent.mkdir(parents=True, exist_ok=True)
        _logger.addHandler(FileHandler(file))


def good(message: str):
    _logger.info(f"{_COLOR_GREEN}{message}{_COLOR_RESET}")


def info(message: str):
    _logger.debug(f"{_COLOR_BLUE}{message}{_COLOR_RESET}")


def warn(message: str):
    _logger.warning(f"{_COLOR_YELLOW}{message}{_COLOR_RESET}")


def bad(message: str):
    _logger.error(f"{_COLOR_YELLOW}{message}{_COLOR_RESET}")


def crit(error: Union[Exception, str]):
    if isinstance(error, str):
        error = Exception(error)
    _logger.critical(f"{_COLOR_YELLOW}{error}{_COLOR_RESET}")
    exit(str(error))
