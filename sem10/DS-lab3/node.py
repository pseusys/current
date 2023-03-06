from asyncio import Future
from json import loads
from typing import Dict
from uuid import uuid4

from aio_pika import connect
from aio_pika.abc import AbstractIncomingMessage

from parser import make_rule, make_action
from utils import error


class Node:
    def __init__(self, name: str, raw: Dict):
        self.name = name
        self.id = uuid4()

        self.connection = None
        self.channel = None

        self.one_shot = raw["one_shot"] if "one_shot" in raw else False
        self.in_channels = raw["in_channels"] if "in_channels" in raw else list()
        self.out_channels = raw["out_channels"] if "out_channels" in raw else list()

        self.variables = raw["variables"] if "variables" in raw else dict()
        self._populate_special_variables()

        if "rules" in raw:
            self.rules = [make_rule(name, rule) for name, rule in raw["rules"].items()]
        else:
            self.rules = list()

        if "init" in raw:
            self.init = [make_action(name, action) for name, action in raw["init"].items()]
        else:
            self.init = list()

    def _populate_special_variables(self):
        self.variables["NODE_ID"] = self.id

    async def consume(self, message: AbstractIncomingMessage):
        data = loads(message.body.decode("utf-8"))
        accepted = None
        for rule in self.rules:
            application = await rule(self, message.routing_key, data)
            if application:
                if accepted is None:
                    accepted = application
                else:
                    error(f"Two rules (`{accepted}` and `{application}`) applied for one message ({message})!")

    async def launch(self):
        self.connection = await connect("amqp://guest:guest@localhost/")
        self.channel = await self.connection.channel()
        for i in range(len(self.in_channels)):
            self.in_channels[i] = await self.channel.declare_queue(self.in_channels[i])
        for i in range(len(self.out_channels)):
            self.out_channels[i] = await self.channel.declare_queue(self.out_channels[i])

    async def start(self):
        for start in self.init:
            print(f"passing for message {self.name}...")
            await start(self, dict())
        if self.one_shot:
            await self.connection.close()
            return

        for ch in self.in_channels:
            print(f"waiting for message {self.name}...")
            await ch.consume(self.consume, no_ack=True)
        await Future()
