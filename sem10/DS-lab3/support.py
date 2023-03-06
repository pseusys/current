from typing import Any, Dict, ForwardRef

from utils import error

Node = ForwardRef("Node", is_class=True)


class Object(object):
    pass


def get_raw_or_variable(state: Node, message: Dict, value: Any) -> Any:
    if not isinstance(value, str):
        return value

    arr = value.split(".")
    if len(arr) == 1:
        return value

    if arr[0] == "self":
        if arr[1] in state.variables.keys():
            return state.variables[arr[1]]
        else:
            error(f"Variable '{arr[1]}' of instance {arr[0]} not set!")
    elif arr[0] == "message":
        if arr[1] in message.keys():
            return message[arr[1]]
        else:
            error(f"Variable '{arr[1]}' of instance {message} not set!")
    else:
        error(f"Instance {arr[0]} not defined!")
