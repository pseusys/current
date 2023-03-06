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
        self.one_shot = raw["one_shot"] if "one_shot" in raw else False
        self.in_channels = {channel: None for channel in raw["in_channels"]} if "in_channels" in raw else dict()
        self.out_channels = {channel: None for channel in raw["out_channels"]} if "out_channels" in raw else dict()

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

    def consume(self, message: AbstractIncomingMessage):
        print(message.channel)
        print(message.body)
        """
        accepted = None
        for rule in self.rules:
            application = rule.apply(self, channel, message)
            if application:
                if accepted is None:
                    accepted = application
                else:
                    error(f"Two rules (`{accepted}` and `{application}`) applied for one message ({message})!")
        """

    async def launch(self):
        self.connection = await connect("amqp://guest:guest@localhost/")
        async with self.connection:
            channel = await self.connection.channel()
            for ch in self.in_channels.keys():
                self.in_channels[ch] = await channel.declare_queue(ch)
            for ch in self.out_channels.keys():
                self.out_channels[ch] = await channel.declare_queue(ch)

    async def start(self):
        async with self.connection:
            for ch in self.in_channels.values():
                await ch.consume(self.consume, no_ack=True)
            for start in self.init:
                start()
