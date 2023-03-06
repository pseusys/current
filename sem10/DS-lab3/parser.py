from typing import Dict, Callable, Optional, ForwardRef

from support import get_raw_or_variable
from utils import error

Node = ForwardRef("Node", is_class=True)


def make_send(raw: Dict) -> Callable[[Node, Dict], None]:
    if "channel" not in raw:
        error("In send block 'channel' field is not defined - the field is required for sending messages!")
    on = raw["channel"]

    if "message" not in raw:
        error("In send block 'message' field is not defined - the field is required for sending message!")
    message = raw["message"]

    def send(state: Node, rec: Dict):
        if on not in state.out_channels.keys():
            error(f"Can't send message to not output channel `{on}`!")
        msg = dict()
        for var, val in message.items():
            msg[var] = get_raw_or_variable(state, rec, val)
        print(f"Writing {on} -> {msg}")

    return send


def make_assign(raw: Dict) -> Callable[[Node, Dict], None]:
    def assign(state: Node, rec: Dict):
        upd = dict()
        for var, val in raw.items():
            upd[var] = get_raw_or_variable(state, rec, val)
        print(f"Updating -> {upd}")

    return assign


def make_action(name: str, raw: Dict) -> Callable[[Node, Dict], None]:
    if name == "send":
        return make_send(raw)
    elif name == "set":
        return make_assign(raw)
    else:
        error(f"Action `{name}` unknown!")


def make_condition(raw: str) -> Callable[[Node, Dict], bool]:
    def evaluate(state: Node, rec: Dict):
        state_obj = object()
        for var, val in state.variables.items():
            setattr(state_obj, var, val)
        msg_obj = object()
        for var, val in rec.items():
            setattr(msg_obj, var, val)
        return exec(raw, {}, {"self": state_obj, "message": msg_obj})

    return evaluate


def make_rule(name: str, raw: Dict) -> Callable[[Node, str, Dict], Optional[str]]:
    if "channel" not in raw:
        error(f"In the rule `{name}` 'channel' field is not defined - the field is required for message-based events!")
    on = raw["channel"]

    if "if" in raw:
        condition = make_condition(raw["if"])
    else:
        condition = None

    if "send" in raw:
        send = make_send(raw["send"])
    else:
        send = None

    if "set" in raw:
        assign = make_assign(raw["set"])
    else:
        assign = None

    def apply(state: Node, channel: str, message: Dict) -> Optional[str]:
        if channel != on or condition is None or not condition(state, message):
            return None
        if on not in state.in_channels.keys():
            error(f"Can't receive message from not input channel `{on}`!")
        if send is not None:
            send(state, message)
        if assign is not None:
            assign(state, message)
        return name

    return apply
