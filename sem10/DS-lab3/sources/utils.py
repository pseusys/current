from typing import Iterable, Any, Callable


class Object(object):
    pass


def make_list(iterable: Iterable[Any]) -> str:
    return f"[{', '.join(map(lambda x: repr(x), iterable))}]"


class RepresentativeFunction:
    def __init__(self, function: Callable, represent: Callable[[Callable], str]):
        self.function = function
        self.represent = represent

    def __call__(self, *args, **kwargs):
        return self.function(*args, **kwargs)

    def __repr__(self):
        return self.represent(self.function)


def representation(represent: Callable[[Callable], str]):
    def representation_decorator(function: Callable):
        return RepresentativeFunction(function, represent)
    return representation_decorator
