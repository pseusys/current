package main

import (
	"container/list"
	"fmt"
	"os"
)

const (
	SVG_BACK_COLOR = "white"
	SVG_WALL_COLOR = "black"
	SVG_DOOR_COLOR = "burlywood"
	SVG_PATH_COLOR = "red"
	SVG_NONE_COLOR = "none"
)

var (
	SVG_WIDTH  uint
	SVG_HEIGHT uint
)

func svgHeader(file *os.File) (int, error) {
	str := fmt.Sprintf("<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" width=\"%d\" height=\"%d\">\n", SVG_WIDTH, SVG_HEIGHT)
	return file.WriteString(str)
}

func svgFooter(file *os.File) (int, error) {
	return file.WriteString("</svg>\n")
}

func svgLine(file *os.File, x1 float64, y1 float64, x2 float64, y2 float64, stroke string, width float64) (int, error) {
	str := fmt.Sprintf("\t<line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\" stroke=\"%s\" style=\"stroke-width:%f;\"/>\n", x1, y1, x2, y2, stroke, width)
	return file.WriteString(str)
}

func (r *Room) svgWall(file *os.File) (int, error) {
	var x1, y1, x2, y2 float64
	if r.doorHorizontal() {
		start_y := float64(r.door_y) + 3.0
		y1 = start_y
		y2 = start_y
		x1 = float64(r.x) + 3.0
		x2 = x1 + float64(r.width)
	} else {
		start_x := float64(r.door_x) + 3.0
		x1 = start_x
		x2 = start_x
		y1 = float64(r.y) + 3.0
		y2 = y1 + float64(r.height)
	}
	return svgLine(file, x1, y1, x2, y2, SVG_WALL_COLOR, 0.25)
}

func (r *Room) svgDoor(file *os.File) (int, error) {
	var x1, y1, x2, y2 float64
	if r.doorHorizontal() {
		start_y := float64(r.door_y) + 3.0
		y1 = start_y
		y2 = start_y
		x1 = float64(r.door_x) + 3.1
		x2 = x1 + 0.8
	} else {
		start_x := float64(r.door_x) + 3.0
		x1 = start_x
		x2 = start_x
		y1 = float64(r.door_y) + 3.1
		y2 = y1 + 0.8
	}
	return svgLine(file, x1, y1, x2, y2, SVG_DOOR_COLOR, 0.5)
}

func svgRect(file *os.File, x float64, y float64, w float64, h float64, stroke string, fill string) (int, error) {
	str := fmt.Sprintf("\t<rect x=\"%f\" y=\"%f\" width=\"%f\" height=\"%f\" fill=\"%s\" stroke=\"%s\" style=\"stroke-width:0.25;\"/>\n", x, y, w, h, fill, stroke)
	return file.WriteString(str)
}

func (r *Room) svgOutline(file *os.File) (int, error) {
	return svgRect(file, float64(r.x)+3.0, float64(r.y)+3.0, float64(r.width), float64(r.height), SVG_WALL_COLOR, SVG_NONE_COLOR)
}

func (m *Maze) SaveSVG(name string, path *list.List) error {
	file, err := os.Create(name)
	if err != nil {
		return JoinError("error opening file", err)
	}
	defer file.Close()

	SVG_WIDTH = m.tree.width + 6
	SVG_HEIGHT = m.tree.height + 6
	_, err = svgHeader(file)
	if err != nil {
		return JoinError("error writing header", err)
	}

	_, err = svgRect(file, 0, 0, float64(SVG_WIDTH), float64(SVG_HEIGHT), SVG_NONE_COLOR, SVG_BACK_COLOR)
	if err != nil {
		return JoinError("error clearing background", err)
	}
	_, err = m.tree.svgOutline(file)
	if err != nil {
		return JoinError("error drawing outline", err)
	}

	for i := 0; i < int(m.rooms); i++ {
		room := m.array[i]
		if room.isFinal() {
			continue
		}
		_, err = room.svgWall(file)
		if err != nil {
			return JoinError(fmt.Sprintf("error writing room #%d", room.findRoomIndex(m.array)), err)
		}
		_, err = room.svgDoor(file)
		if err != nil {
			return JoinError(fmt.Sprintf("error writing door #%d", room.findRoomIndex(m.array)), err)
		}
	}

	if path.Len() > 1 {
		prev_x := -1.0
		prev_y := -1.0
		for e := path.Front(); e != nil; e = e.Next() {
			current := e.Value.(*Room)
			var current_x, current_y float64
			if current.isFinal() {
				current_x = float64(current.x) + float64(current.width)/2.0 + 3.0
				current_y = float64(current.y) + float64(current.height)/2.0 + 3.0
			} else {
				current_x = float64(current.door_x) + 3.0
				current_y = float64(current.door_y) + 3.0
				if current.doorHorizontal() {
					current_x = current_x + 0.5
				} else {
					current_y = current_y + 0.5
				}
			}
			if prev_x != -1 && prev_y != -1 {
				_, err = svgLine(file, prev_x, prev_y, current_x, current_y, SVG_PATH_COLOR, 0.1)
				if err != nil {
					return JoinError("error writing path", err)
				}
			}
			prev_x = current_x
			prev_y = current_y
		}
	}

	_, err = svgFooter(file)
	if err != nil {
		return JoinError("error writing footer", err)
	}
	return nil
}
