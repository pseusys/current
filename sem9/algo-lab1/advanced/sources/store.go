package main

import (
	"bytes"
	"encoding/gob"
)

const UNDEFINED_POINTER = -1

func (r *Room) findRoomIndex(rooms []*Room) int {
	for i := 0; i < len(rooms); i++ {
		if rooms[i] == r {
			return i
		}
	}
	return UNDEFINED_POINTER
}

func denotePointer(n int, rooms []*Room) *Room {
	if n != UNDEFINED_POINTER {
		return rooms[n]
	} else {
		return nil
	}
}

type compactRoom struct {
	X, Y, Width, Height, Door_x, Door_y uint
	Left_child, Right_child, Parent     int
}

func (r *compactRoom) expandRoom(current *Room, rooms []*Room) {
	*current = Room{
		r.X,
		r.Y,
		r.Width,
		r.Height,
		r.Door_x,
		r.Door_y,
		denotePointer(r.Left_child, rooms),
		denotePointer(r.Right_child, rooms),
		denotePointer(r.Parent, rooms),
	}
}

func (r *Room) collapseRoom(rooms []*Room) *compactRoom {
	compact := compactRoom{
		r.x,
		r.y,
		r.width,
		r.height,
		r.door_x,
		r.door_y,
		r.left_child.findRoomIndex(rooms),
		r.right_child.findRoomIndex(rooms),
		r.parent.findRoomIndex(rooms),
	}
	return &compact
}

type compactMaze struct {
	Rooms uint
	Array []compactRoom
}

func (m *compactMaze) expandMaze() *Maze {
	array := make([]*Room, m.Rooms)
	for i := 0; i < int(m.Rooms); i++ {
		array[i] = &Room{}
	}
	for i := 0; i < int(m.Rooms); i++ {
		m.Array[i].expandRoom(array[i], array)
		array[i].PrintRoom(i)
	}
	maze := Maze{
		m.Rooms,
		array[0],
		array,
	}
	return &maze
}

func (m *Maze) collapseMaze() *compactMaze {
	maze := compactMaze{
		m.rooms,
		make([]compactRoom, m.rooms),
	}
	for i := 0; i < int(m.rooms); i++ {
		maze.Array[i] = *m.array[i].collapseRoom(m.array)
	}
	return &maze
}

func ParseMaze(data []byte) (*Maze, error) {
	var compact compactMaze
	buffer := bytes.NewBuffer(data)
	decoder := gob.NewDecoder(buffer)
	err := decoder.Decode(&compact)
	if err != nil {
		return nil, JoinError("maze decoding error", err)
	}
	return compact.expandMaze(), nil
}

func (m *Maze) DumpMaze() ([]byte, error) {
	var buffer bytes.Buffer
	encoder := gob.NewEncoder(&buffer)
	err := encoder.Encode(m.collapseMaze())
	if err != nil {
		return nil, JoinError("maze encoding error", err)
	}
	return buffer.Bytes(), nil
}
