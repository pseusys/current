from library import read_input, split_courses


if __name__ == "__main__":
    room_capacity, course_list, matrix = read_input()
    occupation_list = sorted(split_courses(room_capacity, course_list), key=lambda c: c.start_time)

    room_list = list()
    for occupation in occupation_list:
        inserted = False
        for room_index in range(len(room_list)):
            room = room_list[room_index]
            if occupation.start_time >= room.end_time + matrix.gap(room.course_label, occupation.course_label):
                room_list[room_index] = occupation
                inserted = True
                break
        if not inserted:
            room_list += [occupation]
    
    print(len(room_list))
