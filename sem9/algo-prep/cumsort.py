from typing import List


class StableChar:
    def __init__(self, char: str, payload: int):
        self.base = char
        self.payload = payload

    def __repr__(self):
        return self.base

    def __eq__(self, char):
        return self.base == char.base
    
    def __lt__(self, char):
        return self.base < char.base
    
    def __le__(self, char):
        return self.base <= char.base

    def __gt__(self, char):
        return self.base > char.base
    
    def __ge__(self, char):
        return self.base >= char.base

    def __str__(self) -> str:
        return f"StableChar(b: {self.base}, p: {self.payload})"


def log(logging: bool, message: str):
    if logging:
        print(message)


def cumsort(array: List, shrink: float = 1.3, logging: bool = False) -> List:
    """
    >>> arr = list("MUSICAL")
    >>> cumsort(arr) == sorted(arr)
    True
    """

    operational = list(array)
    log(logging, "Cumsort initialized")
    gap = len(operational)
    sorted = False

    iter_counter = 0
    while not sorted:
        log(logging, f"\tIteration {iter_counter}:")
        gap = int(gap // shrink)
        log(logging, f"\t\tGap = {gap}")
        if gap <= 1:
            gap = 1
            log(logging, "\t\t\tGap collapsed to 0!")
            sorted = True

        log(logging, f"\t\tArray: {operational}")
        for i in range(len(operational) - gap):
            log(logging, f"\t\tPassage {i}: i = {i}, i+gap = {i+gap}")
            if operational[i]> operational[i+gap]:
                log(logging, "\t\t\tSwapped!")
                operational[i], operational[i+gap] = operational[i+gap], operational[i]
                sorted = False
            log(logging, f"\t\tArray: {operational}")
            i += 1
        iter_counter += 1
    log(logging, "Cumsort finished")
    return operational


if __name__ == "__main__":
    stable_test = [StableChar(c, i) for i, c in enumerate("AMUSEMENT")]
    chars = [f"{char}" for char in cumsort(stable_test, logging=True)]
    stables = [f"{stable}" for stable in sorted(stable_test)]
    assert "".join(chars) == "".join(stables), "Cumsort has proven to be unstable!"
