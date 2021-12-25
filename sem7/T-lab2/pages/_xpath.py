from enum import Enum


class Tag(Enum):
    Any = '*'
    Link = 'a'
    Button = 'button'
    Summary = 'summary'
    List = 'ul'
    Input = 'input'
    Div = 'div'
    Elem = 'li'
    Par = 'p'
    Span = 'span'


class Param(Enum):
    Link = '@href'
    Type = '@type'
    Class = '@class'
    Text = 'text()'
    Name = '@name'
    Id = '@id'

    Label = '@aria-label'
    DataIterable = '@data-filterable-for'


class Condition(Enum):
    Contains = 0
    Equals = 1


def _compose(param: Param, cond: Condition, val: str):
    return f"{param.value}='{val}'" if cond is Condition.Equals else f"contains({param.value}, '{val}')"


def create(tag: Tag, *pairs: tuple[Param, Condition, str]):
    return f"//{tag.value}[{' and '.join(map(lambda tup: _compose(*tup), pairs))}]"
