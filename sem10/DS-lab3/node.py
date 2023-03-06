from json import loads, dumps
from typing import Dict, Mapping, Awaitable, List, Any, Callable, Optional

from aio_pika import connect, Message
from aio_pika.abc import AbstractIncomingMessage

from utils import error, Object


class Node:
    def __init__(self, name: str, identifier: int, template: Dict, port_mapping: Mapping[str, str]):
        self.name = name
        self.id = identifier

        self.connection = None
        self.channel = None

        self.in_channels = {ch: port_mapping[ch] for ch in template.get("in_channels", list())}
        self.out_channels = {ch: port_mapping[ch] for ch in template.get("out_channels", list())}

        self.one_shot = template.get("one_shot", False)
        self.variables = template.get("variables", dict())
        self._populate_special_variables()

        if "rules" in template:
            self.rules = [self._make_rule(name, rule) for name, rule in template["rules"].items()]
        else:
            self.rules = list()

        if "init" in template:
            self.init = [self._make_action(name, action) for name, action in template["init"].items()]
        else:
            self.init = list()

    def _populate_special_variables(self):
        self.variables["id"] = self.id

    def _make_send(self, raw: Dict) -> Callable[[Dict], Awaitable[None]]:
        if "channel" not in raw:
            error("In send block 'channel' field is not defined - the field is required for sending messages!")
        on = raw["channel"]

        if "message" not in raw:
            error("In send block 'message' field is not defined - the field is required for sending message!")
        message = raw["message"]

        async def send(rec: Dict):
            if on not in self.out_channels.keys():
                error(f"Can't send message to not output channel `{on}`!")
            msg = dict()
            for var, val in message.items():
                msg[var] = self._get_raw_or_variable(rec, val)
            target_queue = self.out_channels[on].name
            mess = dumps(msg).encode("utf-8")
            print(self.name, "sending message", mess)
            await self.channel.default_exchange.publish(Message(mess), routing_key=target_queue)

        return send

    def _make_assign(self, raw: Dict) -> Callable[[Dict], Awaitable[None]]:
        async def assign(rec: Dict):
            upd = dict()
            for var, val in raw.items():
                upd[var] = self._get_raw_or_variable(rec, val)
            print(f"({self.name}) Updating -> {upd}")

        return assign

    def _make_action(self, name: str, raw: Dict) -> Callable[[Dict], Awaitable[None]]:
        if name == "send":
            return self._make_send(raw)
        elif name == "set":
            return self._make_assign(raw)
        else:
            error(f"Action `{name}` unknown!")

    def _make_condition(self, raw: str) -> Callable[[Dict], bool]:
        def evaluate(rec: Dict):
            state_obj = Object()
            for var, val in self.variables.items():
                setattr(state_obj, var, val)
            msg_obj = Object()
            for var, val in rec.items():
                setattr(msg_obj, var, val)
            return bool(eval(raw, {}, {"self": state_obj, "message": msg_obj}))

        return evaluate

    def _make_rule(self, name: str, raw: Dict) -> Callable[[str, Dict], Awaitable[Optional[str]]]:
        if "channel" not in raw:
            error(f"In the rule `{name}` 'channel' field is not defined - the field is required for message-based events!")
        on = raw["channel"]

        if "if" in raw:
            condition = self._make_condition(raw["if"])
        else:
            condition = None

        if "send" in raw:
            send = self._make_send(raw["send"])
        else:
            send = None

        if "set" in raw:
            assign = self._make_assign(raw["set"])
        else:
            assign = None

        async def apply(channel: str, message: Dict) -> Optional[str]:
            if channel != on or condition is None or not condition(message):
                return None
            if on not in self.in_channels.keys():
                error(f"Can't receive message from not input channel `{on}`!")
            if send is not None:
                await send(message)
            if assign is not None:
                await assign(message)
            return name

        return apply

    def _get_raw_or_variable(self, message: Dict, value: Any) -> Any:
        if not isinstance(value, str):
            return value

        arr = value.split(".")
        if len(arr) == 1:
            return value

        if arr[0] == "self":
            if arr[1] in self.variables.keys():
                return self.variables[arr[1]]
            else:
                error(f"Variable '{arr[1]}' of instance {arr[0]} not set!")
        elif arr[0] == "message":
            if arr[1] in message.keys():
                return message[arr[1]]
            else:
                error(f"Variable '{arr[1]}' of instance {message} not set!")
        else:
            error(f"Instance {arr[0]} not defined!")

    async def _consume(self, message: AbstractIncomingMessage):
        print(self.name, "consuming from", message.routing_key)
        channel = next((name for name, ch in self.in_channels.items() if ch.name == message.routing_key), None)
        if channel is None:
            return

        data = loads(message.body.decode("utf-8"))
        print("consuming", data, self.id, self.name)
        accepted = None
        for rule in self.rules:
            application = await rule(channel, data)
            if application:
                if accepted is None:
                    accepted = application
                else:
                    error(f"Two rules (`{accepted}` and `{application}`) applied for one message ({message})!")

        if accepted is not None:
            print(self.name, "accepting message!")
            await message.ack()
        else:
            print(self.name, "rejecting message!")
            await message.nack(requeue=False)

    async def launch(self):
        self.connection = await connect("amqp://guest:guest@localhost/")
        self.channel = await self.connection.channel()
        for q_name in list(self.in_channels.keys()):
            print(self.name, "setting in", self.in_channels[q_name])
            self.in_channels[q_name] = await self.channel.declare_queue(self.in_channels[q_name], auto_delete=True)
        for q_name in list(self.out_channels.keys()):
            print(self.name, "setting out", self.out_channels[q_name])
            self.out_channels[q_name] = await self.channel.declare_queue(self.out_channels[q_name], auto_delete=True)

    async def start(self) -> List[Awaitable]:
        print("starting", self.name, "as", self.one_shot)
        for start in self.init:
            print(f"passing for message {self.name}...")
            await start(dict())
        if self.one_shot:
            return [self.connection.close()]

        print(f"waiting for message {self.name} on {list(ch.name for ch in self.in_channels.values())}...")
        return [ch.consume(self._consume) for ch in self.in_channels.values()]
