package main

import (
	"container/list"
	"fmt"
)

func (r *Room) pointIncluded(x uint, y uint) bool {
	return x >= r.x && x < r.x+r.width && y >= r.y && y < r.y+r.height
}

func (m *Maze) downfall(start *Room, current *Room) (*Room, error) {
	switch {
	case current.isFinal():
		return current, nil
	case current.left_child.doorIncluded(start.door_x, start.door_y, start.doorHorizontal()):
		return m.downfall(start, current.left_child)
	case current.right_child.doorIncluded(start.door_x, start.door_y, start.doorHorizontal()):
		return m.downfall(start, current.right_child)
	}
	current_index := fmt.Sprintf("current %d", current.findRoomIndex(m.array))
	door_index := fmt.Sprintf("door %d", start.findRoomIndex(m.array))
	return nil, JoinError("unexpected door position", current_index, door_index)
}

func (m *Maze) pathToRoot(current *Room, previous *Room, result *list.List) (*list.List, error) {
	switch {
	case current == m.tree:
		result.PushBack(current)
		return result, nil
	case current.isFinal():
		result.PushBack(current)
		fallthrough
	case previous == current.right_child:
		return m.pathToRoot(current.parent, current, result)
	case previous == current.left_child:
		result.PushBack(current)
		bottom, err := m.downfall(current, current.right_child)
		if err != nil {
			return nil, JoinError("downfall error", err)
		} else {
			return m.pathToRoot(bottom, current, result)
		}
	}
	current_index := fmt.Sprintf("current %d", current.findRoomIndex(m.array))
	previous_index := fmt.Sprintf("previous %d", previous.findRoomIndex(m.array))
	return nil, JoinError("unexpected path", current_index, previous_index)
}

func (m *Maze) BuildPath(x1 uint, y1 uint, x2 uint, y2 uint) (*list.List, error) {
	var start, end int
	for i := 0; i < len(m.array); i++ {
		if !m.array[i].isFinal() {
			continue
		} else if m.array[i].pointIncluded(x1, y1) {
			start = i
		} else if m.array[i].pointIncluded(x2, y2) {
			end = i
		}
	}

	LogF("Building path from room #%d to room #%d\n", start, end)

	start_path, err := m.pathToRoot(m.array[start], nil, list.New())
	if err != nil {
		return nil, JoinError("start building path error", err)
	}
	LogF("Path from start to root:\n\t")
	for e := start_path.Front(); e != nil; e = e.Next() {
		LogF("#%d -> ", e.Value.(*Room).findRoomIndex(m.array))
	}
	LogF("(root)\n")

	end_path, err := m.pathToRoot(m.array[end], nil, list.New())
	if err != nil {
		return nil, JoinError("end building path error", err)
	}
	LogF("Path from end to root:\n\t")
	for e := end_path.Front(); e != nil; e = e.Next() {
		LogF("#%d -> ", e.Value.(*Room).findRoomIndex(m.array))
	}
	LogF("(root)\n")

	// TODO: merge start and end paths

	return start_path, nil
}
