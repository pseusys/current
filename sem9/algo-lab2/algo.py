from re import compile
from typing import Tuple


candyexp = compile(r"[1|0]+")


def separate(string: str) -> Tuple[int, int, int]:
    """
    Separate 0s from 1s in a string.
    Calculates number of ones and zeros in a string.
    :return: tuple, first element is number of 0s, second is number of 1s, third is total length.
    """
    result = [0, 0]
    for char in string:
        result[0 if char == "0" else 1] += 1
    return *result, len(string)


def calculate(string: str) -> int:
    """
    Calculate maximum sellable candy length.
    """
    table = [0] * len(string)
    for n in range(len(string)):
        for i in range(n + 1):
            tail_zeros, tail_ones, total = separate(string[i:n + 1])
            tail = total if tail_ones > tail_zeros else 0
            result = table[i - 1] + tail
            if result > table[n]:
                table[n] = result
    return table[-1]


if __name__ == "__main__":
    string = input("Enter the candy representation: ")
    if candyexp.fullmatch(string):
        print(calculate(string))
    else:
        print("Wrong string format!")
