from math import ceil
from typing import List

from maxflow import Graph

from library import Course, Matrix, read_input


def calculate_maximum_rooms(course_list: List[Course], room_capacity: int):
    return sum([ceil(course.student_number / room_capacity) for course in course_list])


def build_course_graph(course_list: List[Course], matrix: Matrix, room_capacity: int):
    graph = Graph[int](len(course_list) * 2, len(course_list) * (len(course_list) + 2))

    course_nodes = list()
    for course in course_list:
        course_end, course_start = graph.add_nodes(2)
        graph.add_tedge(course_end, course.end_time, 0)
        graph.add_tedge(course_start, 0, course.start_time)
        course_nodes += [(course_end, course_start)]

    zipped_lists = list(zip(course_list, course_nodes))
    for first_idx, (first_course, (first_course_end, _)) in enumerate(zipped_lists):
        for second_idx, (second_course, (_, second_course_start)) in enumerate(zipped_lists):
            if second_course.start_time >= first_course.end_time + matrix.gap(first_idx, second_idx):
                graph.add_edge(first_course_end, second_course_start, ceil(first_course.student_number / room_capacity), 0)

    return graph


if __name__ == "__main__":
    room_capacity, course_list, matrix = read_input()
    maximum_rooms = calculate_maximum_rooms(course_list, room_capacity)
    graph = build_course_graph(course_list, matrix, room_capacity)
    print(maximum_rooms - graph.maxflow())
