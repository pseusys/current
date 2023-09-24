package main

import (
	"errors"
)

type Room struct {
	x, y, width, height, door_x, door_y uint
	left_child, right_child, parent     *Room
}

func (r *Room) doorHorizontal() bool {
	return r.height > r.width
}

func (r *Room) doorIncluded(room_door *Room) bool {
	if room_door.doorHorizontal() {
		return room_door.door_x >= r.x && room_door.door_x < r.x+r.width && (room_door.door_y == r.y || room_door.door_y == r.y+r.height)
	} else {
		return room_door.door_y >= r.y && room_door.door_y < r.y+r.height && (room_door.door_x == r.x || room_door.door_x == r.x+r.width)
	}
}

func (r *Room) isFinal() bool {
	return r.width == 1 || r.height == 1
}

func (r *Room) divideRecursive(pivot *Room) (uint, error) {
	if r.isFinal() {
		return 1, nil
	}

	first_child := Room{
		x:      r.x,
		y:      r.y,
		parent: r,
	}
	second_child := Room{
		parent: r,
	}

	height_division := (RandUint() % (r.height - 1)) + 1
	width_division := (RandUint() % (r.width - 1)) + 1
	r.door_x = r.x + width_division
	r.door_y = r.y + height_division

	if r.doorHorizontal() {
		second_child.x = r.x
		second_child.y = r.y + height_division
		first_child.width = r.width
		first_child.height = height_division
		second_child.width = r.width
		second_child.height = r.height - height_division
	} else {
		second_child.x = r.x + width_division
		second_child.y = r.y
		first_child.width = width_division
		first_child.height = r.height
		second_child.width = r.width - width_division
		second_child.height = r.height
	}

	if pivot != nil {
		if first_child.doorIncluded(pivot) {
			r.right_child = &first_child
			r.left_child = &second_child
		} else if second_child.doorIncluded(pivot) {
			r.right_child = &second_child
			r.left_child = &first_child
		} else {
			return 0, errors.New("entrance doesn't belong to children")
		}
	} else {
		pivot = r
		r.right_child = &first_child
		r.left_child = &second_child
	}

	right_weight, err := r.right_child.divideRecursive(pivot)
	if err != nil {
		return 0, JoinError("invalid right subtree", err)
	}
	left_weight, err := r.left_child.divideRecursive(r)
	if err != nil {
		return 0, JoinError("invalid left subtree", err)
	}
	return right_weight + left_weight + 1, nil
}

func (r *Room) PrintRoom(index int) {
	LogF("Room #%d: (x: %d, y: %d) - (w: %d, h:%d)\n", index, r.x, r.y, r.width, r.height)
	if !r.isFinal() {
		LogF("\thas a door: (x: %d, y: %d), horizontal: %v.\n", r.door_x, r.door_y, r.doorHorizontal())
	} else {
		LogF("\tis a leaf room.\n")
	}
}

func (r *Room) collectRooms(counter uint, array []*Room) uint {
	array[counter] = r
	r.PrintRoom(int(counter))

	if r.left_child != nil {
		counter = r.left_child.collectRooms(counter+1, array)
	}
	if r.right_child != nil {
		counter = r.right_child.collectRooms(counter+1, array)
	}

	return counter
}

type Maze struct {
	rooms uint
	tree  *Room
	array []*Room
}

func GenerateMaze(w uint, h uint) (*Maze, error) {
	maze := &Maze{}
	maze.tree = &Room{
		width:  w,
		height: h,
	}

	var err error
	maze.rooms, err = maze.tree.divideRecursive(nil)
	if err != nil {
		return nil, JoinError("tree generation error", err)
	}

	maze.array = make([]*Room, maze.rooms)
	maze.tree.collectRooms(0, maze.array)
	return maze, err
}
