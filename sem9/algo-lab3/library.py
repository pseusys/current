from dataclasses import dataclass
from math import ceil
from typing import List, Tuple


class Course:
    def __init__(self, line: str):
        self.start_time, self.end_time, self.student_number = [int(num) for num in line.split()]

    def __repr__(self) -> str:
        return f"Course(start_time: {self.start_time}, end_time: {self.end_time}, student_number: {self.student_number})"


@dataclass
class Occupation:
    start_time: int
    end_time: int
    course_label: int

    @classmethod
    def from_course(cls, course: Course, index: int) -> "Occupation":
        return cls(course.start_time, course.end_time, index)

    def __repr__(self) -> str:
        return f"Occupation(start_time: {self.start_time}, end_time: {self.end_time}, course_label: {self.course_label})"


class Matrix:
    def __init__(self, lines: List[str]):
        self.matrix = [[int(num) for num in line.split()] for line in lines]

    def gap(self, ending_course: int, beginning_course: int) -> int:
        return self.matrix[ending_course][beginning_course]

    def __repr__(self) -> str:
        matrix_string = "\n\t".join([", ".join([str(value) for value in line]) for line in self.matrix])
        return f"Matrix([\n\t{matrix_string}\n])"


def read_input() -> Tuple[int, List[Course], Matrix]:
    course_number, room_capacity = [int(num) for num in input().split()]
    course_list = [Course(input()) for _ in range(course_number)]
    matrix = Matrix([input() for _ in range(course_number)])
    return room_capacity, course_list, matrix


def split_courses(room_capacity: int, course_list: List[Course]) -> List[Occupation]:
    occupation_list = list()
    for course_index, course in enumerate(course_list):
        occupation_list += [Occupation.from_course(course, course_index) for _ in range(ceil(course.student_number / room_capacity))]
    return occupation_list
