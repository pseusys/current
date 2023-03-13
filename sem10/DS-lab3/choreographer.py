from argparse import ArgumentParser
from asyncio import run, gather, Future
from os.path import isfile
from typing import List, Dict

from yaml import safe_load

from node import Node
from utils import error


async def run_nodes(nodes: List[Node]):
    print("launching nodes...")
    await gather(*[node.launch() for index, node in enumerate(nodes)])
    print("running nodes...")
    await gather(*[coro for node in nodes for coro in await node.start()])
    await Future()


def create_node(name: str, identifier: int, data: Dict, templates: Dict) -> Node:
    if "template" not in data.keys():
        error(f"Node `{name}` has no template and thus can't be built!")
    if data["template"] not in templates.keys():
        error(f"Template `{data['template']}` for node `{name}` not found!")

    template = templates[data["template"]]
    channels = template.get("in_channels", list()) + template.get("out_channels", list())
    if len(channels) > 0 and ("channels" not in data or set(data["channels"].keys()) != set(channels)):
        error(f"Not all channels mapped for node `{name}`!")

    return Node(name, identifier, template, data.get("channels", dict()))


if __name__ == '__main__':
    parser = ArgumentParser()
    parser.add_argument("-l", "--logs", help="Logs file to print the process output.")
    parser.add_argument("config", help="Config file to build graph from.")

    args = vars(parser.parse_args())

    if not isfile(args["config"]):
        error(f"File `{args['config']}` can not be opened!")
    with open(args["config"], "r") as conf:
        config = safe_load(conf)

    if "templates" not in config:
        error(f"File `{args['config']}` can not be opened!")
    if "matrix" not in config:
        error(f"File `{args['config']}` can not be opened!")

    graph = [create_node(name, index + 1, data, config["templates"]) for index, (name, data) in enumerate(config["matrix"].items())]
    run(run_nodes(graph))
