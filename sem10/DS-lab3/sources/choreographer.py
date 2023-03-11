from argparse import ArgumentParser
from asyncio import run, gather, Future
from os.path import isfile
from typing import Iterable, Dict

from yaml import safe_load

from node import Node
from logger import setup_logger, crit, info, bad, good
from utils import make_list

_VERBOSENESS = ['DEBUG', 'INFO', 'WARNING', 'ERROR', 'CRITICAL']


def main(raw: Dict):
    setup_logger(_VERBOSENESS[0] if raw["verbose"] is None else raw["verbose"], raw["logs"])
    info("Choreographer started, resolving configuration...")

    if not isfile(raw["config"]):
        crit(f"File `{raw['config']}` can not be opened!")
    with open(raw["config"], "r") as conf:
        config = safe_load(conf)

    if "templates" not in config:
        crit(f"File `{raw['config']}` can not be opened!")
    if "matrix" not in config:
        crit(f"File `{raw['config']}` can not be opened!")
    info("Configuration resolved!")

    info("Instantiating declared nodes...")
    graph = {name: Node.create(name, data, config["templates"]) for name, data in config["matrix"].items()}
    info("Nodes instantiated!")

    task = None
    try:
        good("Running the distributed system...")
        task = run_nodes_async(graph.values())
        run(task)
    except BaseException as e:
        if task is not None:
            task.close()
        if isinstance(e, KeyboardInterrupt):
            good("Execution finished!")
        else:
            bad(f"Execution failed because of:\n{e}")


async def run_nodes_async(nodes: Iterable[Node]):
    node_names = make_list(node.name for node in nodes)
    info(f"Launching nodes {node_names}")
    await gather(*[node.launch() for node in nodes])
    info("Nodes launched!")
    info(f"Starting nodes {node_names}")
    await gather(*[coro for node in nodes for coro in await node.start()])
    info("Nodes started!")
    info("Sleeping...")
    await Future()


if __name__ == '__main__':
    parser = ArgumentParser()
    parser.add_argument("-l", "--logs", help="Logs file to print the process output.")
    parser.add_argument("-v", "--verbose", help=f"Verboseness level (one of {make_list(_VERBOSENESS)}).")
    parser.add_argument("config", help="Config file to build graph from.")

    main(vars(parser.parse_args()))
