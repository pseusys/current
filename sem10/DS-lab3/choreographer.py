from argparse import ArgumentParser
from asyncio import run, gather, create_task, Future
from os.path import isfile
from typing import List

from yaml import safe_load

from node import Node
from utils import error


async def run_nodes(nodes: List[Node]):
    await gather(*[create_task(node.launch()) for node in nodes])
    await gather(*[create_task(node.start()) for node in nodes if not node.one_shot])
    await Future()


if __name__ == '__main__':
    parser = ArgumentParser()
    parser.add_argument("-l", "--logs", help="Logs file to print the process output.")
    parser.add_argument("config", help="Config file to build graph from.")

    args = vars(parser.parse_args())

    if not isfile(args["config"]):
        error(f"File `{args['config']}` can not be opened!")
    with open(args["config"], "r") as conf:
        config = safe_load(conf)

    graph = [Node(name, data) for name, data in config.items()]
    run(run_nodes(graph))
