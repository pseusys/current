from xml.etree.ElementTree import parse
from argparse import ArgumentParser
from pathlib import Path
from sys import exit, stderr


parser = ArgumentParser()
parser.add_argument("file", help="Target file (will be written if '-s' provided and checked if '-c' provided)")
parser.add_argument("-c", "--check", help="'rosnode list' output, will be checked whether all the nodes in provided file are launched")

args = vars(parser.parse_args())
target_file = Path(args["file"])

if args["check"] is not None:
    if not target_file.is_file():
        raise Exception(f"Target file '{target_file}' can not be read!")
    for node in [node.attrib["name"] for node in parse(target_file).iter("node")]:
        assert node in args["check"], f"Node '{node}' not found in provided list '{args['check']}'!"

else:
    parser.print_help(stderr)
    exit(1)
