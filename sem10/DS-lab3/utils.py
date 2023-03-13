from sys import exit


class Object(object):
    pass


def error(message: str):
    exit(f"\u001b[1;31m{message}\u001b[0m")
