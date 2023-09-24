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

func (r *Room) doorIncluded(x uint, y uint, horizontal bool) bool {
	if horizontal {
		return x >= r.x && x < r.x+r.width && (y == r.y || y == r.y+r.height)
	} else {
		return y >= r.y && y < r.y+r.height && (x == r.x || x == r.x+r.width)
	}
}

func (r *Room) isFinal() bool {
	return r.width == 1 || r.height == 1
}

func (r *Room) divideRecursive() (uint, error) {
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

	if r.parent != nil {
		if first_child.doorIncluded(r.parent.door_x, r.parent.door_y, r.parent.doorHorizontal()) {
			r.right_child = &first_child
			r.left_child = &second_child
		} else if second_child.doorIncluded(r.parent.door_x, r.parent.door_y, r.parent.doorHorizontal()) {
			r.right_child = &second_child
			r.left_child = &first_child
		} else {
			return 0, errors.New("entrance doesn't belong to children")
		}
	} else {
		r.right_child = &first_child
		r.left_child = &second_child
	}

	right_weight, err := r.right_child.divideRecursive()
	if err != nil {
		return 0, JoinError("invalid right subtree", err)
	}
	left_weight, err := r.left_child.divideRecursive()
	if err != nil {
		return 0, JoinError("invalid left subtree", err)
	}
	return right_weight + left_weight + 1, nil
}

func (r *Room) collectRooms(counter uint, array []*Room) uint {
	array[counter] = r
	LogF("Room #%d: (x: %d, y: %d) - (w: %d, h:%d)\n", counter, r.x, r.y, r.width, r.height)
	if !r.isFinal() {
		LogF("\thas a door: (x: %d, y: %d), horizontal: %v.\n", r.door_x, r.door_y, r.doorHorizontal())
	} else {
		LogF("\tis a leaf room.\n")
	}

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
	maze.rooms, err = maze.tree.divideRecursive()
	if err != nil {
		return nil, JoinError("tree generation error", err)
	}

	maze.array = make([]*Room, maze.rooms)
	maze.tree.collectRooms(0, maze.array)
	return maze, err
}
