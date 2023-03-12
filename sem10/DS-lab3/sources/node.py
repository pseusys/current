from json import loads, dumps
from typing_extensions import Self
from typing import Dict, Mapping, Awaitable, List, Any, Callable, Optional

from aio_pika import connect, Message
from aio_pika.abc import AbstractIncomingMessage

from logger import bad, info, good, crit, warn
from utils import make_list, representation, Object


class Node:
    _NODE_INCREMENTING_ID = 0

    def __init__(self, name: str, template: Dict, port_mapping: Mapping[str, str]):
        self.name = name
        info(f"Instantiating node '{name}'...")

        self._connection = None
        self._channel = None

        self._in_channels = {ch: port_mapping[ch] for ch in list(template.get("in_pipes", list()))}
        info(f"  ... with input channels: {make_list(self._in_channels)}.")
        self._out_channels = {ch: port_mapping[ch] for ch in list(template.get("out_pipes", list()))}
        info(f"  ... with output channels: {make_list(self._in_channels)}.")

        self._one_shot = template.get("one_shot", False)
        info(f"  ... with one shot property: {self._one_shot}.")
        self._variables = {name: self._populate_special_variable(variable) for name, variable in dict(template.get("variables", dict())).items()}
        info(f"  ... with variables: {make_list(self._variables)}.")

        if "rules" in template:
            self._rules = [self._make_rule(name, rule) for name, rule in template["rules"].items()]
        else:
            self._rules = list()
        info(f"  ... with rules: {make_list(self._rules)}.")

        if "init" in template:
            self._init = [self._make_action(name, action) for name, action in template["init"].items()]
        else:
            self._init = list()
        info(f"  ... with init actions: {make_list(self._init)}.")
        info("  ... done!")

    @classmethod
    def create(cls, node_name: str, node_data: Dict, templates: Dict) -> Self:
        if "template" not in node_data.keys():
            bad(f"Node `{node_name}` has no template and thus can't be built!")
        if node_data["template"] not in templates.keys():
            bad(f"Template `{node_data['template']}` for node `{node_name}` not found!")

        good(f"Creating node '{node_name}' from template '{node_data['template']}'.")
        template = templates[node_data["template"]]
        pipes = template.get("in_pipes", list()) + template.get("out_pipes", list())
        if len(pipes) > 0 and ("pipes" not in node_data or set(node_data["pipes"].keys()) != set(pipes)):
            bad(f"Not all pipes mapped for node `{node_name}`!")

        pipe_mapping = make_list(f"{pipe} -> {node_data['pipes'][pipe]}" for pipe in pipes)
        info(f"  ... with pipes: {pipe_mapping}.")
        return Node(node_name, template, node_data.get("pipes", dict()))

    @classmethod
    def _populate_special_variable(cls, value: Any) -> Any:
        if value == "NODE_ID":
            cls._NODE_INCREMENTING_ID += 1
            return cls._NODE_INCREMENTING_ID
        else:
            return value

    def _make_send(self, raw: Dict) -> Callable[[Dict], Awaitable[None]]:
        if "pipe" not in raw:
            bad("In send block 'pipe' field is not defined - the field is required for sending messages!")
        on = raw["pipe"]

        if "message" not in raw:
            bad("In send block 'message' field is not defined - the field is required for sending message!")
        message = raw["message"]

        @representation(lambda f: f"<send of message {make_list(message.keys())} to '{on}'>")
        async def send(rec: Dict):
            info(f"      Node '{self.name}': sending message to output pipe '{on}'!")
            if on not in self._out_channels.keys():
                bad(f"      Node '{self.name}': can't send message to not output pipe '{on}'!")
            msg = dict()
            for var, val in message.items():
                msg[var] = self._get_raw_or_variable(rec, val)
            target_queue = self._out_channels[on].name
            mess = dumps(msg).encode("utf-8")
            info(f"      Node '{self.name}': sending message {mess} to queue {target_queue}...")
            await self._channel.default_exchange.publish(Message(mess), routing_key=target_queue)

        return send

    def _make_assign(self, raw: Dict) -> Callable[[Dict], Awaitable[None]]:
        @representation(lambda f: f"<assignment of variables {make_list(raw.keys())}>")
        async def assign(rec: Dict):
            upd = dict()
            for var, val in raw.items():
                upd[var] = self._get_raw_or_variable(rec, val)
            info(f"      Node '{self.name}': updated variables to {upd}!")

        return assign

    def _make_action(self, name: str, raw: Dict) -> Callable[[Dict], Awaitable[None]]:
        if name == "send":
            return self._make_send(raw)
        elif name == "set":
            return self._make_assign(raw)
        else:
            bad(f"Action `{name}` unknown!")

    def _make_condition(self, raw: str) -> Callable[[Dict], bool]:
        @representation(lambda f: f"<condition: '{raw}'>")
        def evaluate(rec: Dict):
            state_obj = Object()
            for var, val in self._variables.items():
                setattr(state_obj, var, val)
            msg_obj = Object()
            for var, val in rec.items():
                setattr(msg_obj, var, val)
            return bool(eval(raw, {}, {"self": state_obj, "message": msg_obj}))

        return evaluate

    def _make_rule(self, name: str, raw: Dict) -> Callable[[str, Dict], Awaitable[Optional[str]]]:
        if "pipe" not in raw:
            bad(f"In the rule '{name}' 'pipe' field is not defined - the field is required for message-based events!")
        on = raw["pipe"]

        if "if" in raw:
            condition = self._make_condition(raw["if"])
        else:
            condition = None

        if "actions" in raw:
            actions = [self._make_action(name, action) for name, action in raw["actions"].items()]
        else:
            actions = list()

        @representation(lambda f: f"<rule: {'un' if condition is None else ''}conditional, on '{on}' with actions {make_list(actions)}>")
        async def apply(pipe: str, message: Dict) -> Optional[str]:
            if pipe != on or (condition is not None and not condition(message)):
                cause = "wrong pipe" if pipe != on else f"condition {repr(condition)} not met"
                warn(f"    Node '{self.name}', rule '{name}': rule can't be applied: {cause}!")
                return None
            if on not in self._in_channels.keys():
                bad(f"    Node '{self.name}', rule '{name}': can't receive message from not input pipe '{on}'!")
            for action in actions:
                info(f"    Node '{self.name}', rule '{name}': performing action {repr(action)}...")
                await action(message)
            info(f"    Node '{self.name}', rule '{name}': done!")
            return name

        return apply

    def _get_raw_or_variable(self, message: Dict, value: Any) -> Any:
        if not isinstance(value, str):
            return value

        arr = value.split(".")
        if len(arr) == 1:
            return value

        if arr[0] == "self":
            if arr[1] in self._variables.keys():
                return self._variables[arr[1]]
            else:
                bad(f"Variable '{arr[1]}' of instance {arr[0]} not set!")
        elif arr[0] == "message":
            if arr[1] in message.keys():
                return message[arr[1]]
            else:
                bad(f"Variable '{arr[1]}' of instance {message} not set!")
        else:
            bad(f"Instance {arr[0]} not defined!")

    async def _consume(self, message: AbstractIncomingMessage):
        info(f"Node '{self.name}' consuming message '{message.message_id}' from queue '{message.routing_key}'...")
        pipe = next((name for name, ch in self._in_channels.items() if ch.name == message.routing_key), None)
        if pipe is None:
            crit(f"No input pipe is associated for the node '{self.name}' with queue '{message.routing_key}'!")

        data = loads(message.body.decode("utf-8"))
        good(f"Node '{self.name}' received message '{message.message_id}': '{data}' from input pipe '{pipe}'!")
        accepted = None
        for rule in self._rules:
            application = await rule(pipe, data)
            if application:
                if accepted is None:
                    accepted = application
                else:
                    crit(f"Two rules ('{accepted}' and '{application}') applied for one message '{message.message_id}')!")

        if accepted is not None:
            good(f"Node '{self.name}' accepted message '{message.message_id}' from input pipe '{pipe}'!")
            await message.ack()
        else:
            warn(f"Node '{self.name}' rejected message '{message.message_id}' from input pipe '{pipe}'!")
            await message.nack(requeue=False)

    async def launch(self):
        info(f"Launching node '{self.name}'...")
        self._connection = await connect("amqp://guest:guest@localhost/")
        self._channel = await self._connection.channel()
        for q_name in list(self._in_channels.keys()):
            info(f"  ... '{self.name}' declares queue '{self._in_channels[q_name]}' for input pipe '{q_name}'.")
            self._in_channels[q_name] = await self._channel.declare_queue(self._in_channels[q_name], auto_delete=True)
        for q_name in list(self._out_channels.keys()):
            info(f"  ... '{self.name}' declares queue '{self._out_channels[q_name]}' for output pipe '{q_name}'.")
            self._out_channels[q_name] = await self._channel.declare_queue(self._out_channels[q_name], auto_delete=True)
        info(f"  ... '{self.name}' done!")

    async def start(self) -> List[Awaitable]:
        info(f"Starting node '{self.name}'...")
        for start in self._init:
            info(f"... executing action {repr(start)}.")
            await start(dict())
        if self._one_shot:
            info("... done early (as one shot)!")
            return [self._connection.close()]

        good(f"Node '{self.name}' started!")
        info(f"  ... '{self.name}' listening for messages on queues {make_list(ch.name for ch in self._in_channels.values())}")
        return [ch.consume(self._consume) for ch in self._in_channels.values()]
