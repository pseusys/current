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

func (m *Maze) SprintPath(path *list.List, root bool, verbose bool) (result string) {
	print_elem := func(elem *list.Element) string {
		print_template := "#%d"
		room_pointer := elem.Value.(*Room)
		if verbose {
			if room_pointer.isFinal() {
				print_template = print_template + " (room)"
			} else {
				print_template = print_template + " (door)"
			}
		}
		return fmt.Sprintf(print_template, room_pointer.findRoomIndex(m.array))
	}

	var last_elem *list.Element
	if root {
		last_elem = path.Back()
		result = fmt.Sprintf("%s\n", "(root)")
	} else {
		result = fmt.Sprintf("%s\n", print_elem(path.Back()))
		if path.Len() > 1 {
			last_elem = path.Back().Prev()
		} else {
			last_elem = nil
		}
	}

	for e := last_elem; e != nil; e = e.Prev() {
		result = fmt.Sprintf("%s -> ", print_elem(e)) + result
	}
	return result
}

func (m *Maze) BuildPath(x1 uint, y1 uint, x2 uint, y2 uint) (*list.List, error) {
	// Find the rooms containing start and end coords
	var start, end int
	for i := 0; i < len(m.array); i++ {
		if !m.array[i].isFinal() {
			continue
		}
		if m.array[i].pointIncluded(x1, y1) {
			start = i
		}
		if m.array[i].pointIncluded(x2, y2) {
			end = i
		}
	}

	LogF("Building path from room #%d to room #%d\n", start, end)

	// Find the path from room start to the root
	start_path, err := m.pathToRoot(m.array[start], nil, list.New())
	if err != nil {
		return nil, JoinError("start building path error", err)
	}
	LogF("Path from start to root:\n\t%s", m.SprintPath(start_path, true, false))

	// Find the path from room end to the root
	end_path, err := m.pathToRoot(m.array[end], nil, list.New())
	if err != nil {
		return nil, JoinError("end building path error", err)
	}
	LogF("Path from end to root:\n\t%s", m.SprintPath(end_path, true, false))

	// Remove the common tail elements
	var last_common *Room
	start_val := start_path.Back()
	end_val := end_path.Back()
	for start_val.Value == end_val.Value {
		last_common = start_val.Value.(*Room)
		start_path.Remove(start_val)
		end_path.Remove(end_val)
		start_val = start_path.Back()
		end_val = end_path.Back()
		if start_val == nil || end_val == nil {
			break
		}
	}

	// Merge lists, creating path
	start_path.PushBack(last_common)
	for e := end_path.Back(); e != nil; e = e.Prev() {
		start_path.PushBack(e.Value.(*Room))
	}
	LogF("Path from start to end:\n\t%s", m.SprintPath(start_path, false, false))

	return start_path, nil
}
