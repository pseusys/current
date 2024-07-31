from argparse import ArgumentParser
from pathlib import Path


parser = ArgumentParser()
parser.add_argument("file", help="Target file (will be written if '-s' provided and checked if '-c' provided)")
parser.add_argument("-s", "--source", help="Sources for this launch, string separated by ':' character")
parser.add_argument("-i", "--include", help="Source file names for this launch, separated by ':' character")

args = vars(parser.parse_args())
target_file = Path(args["file"])

with target_file.open("w") as file:
    file.write("<launch>\n")

    if args["include"] is not None:
        for source in [source for source in args["include"].split(":") if source != ""]:
            file.write(f'\t<include file="config/{source}"/>\n')

    if args["source"] is not None:
        for package, node in [target.split("/") for target in args["source"].split(":")]:
            file.write(f'\t<node name="{Path(node).stem}" pkg="{package}" type="{node}" output="screen" />\n')

    file.write("</launch>\n")
