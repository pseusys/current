package main

import (
	"fmt"
	"os"
)

const (
	SVG_BACK_COLOR = "white"
	SVG_WALL_COLOR = "black"
	SVG_DOOR_COLOR = "burlywood"
	SVG_NONE_COLOR = "none"
)

func writeHeader(file *os.File, width uint, height uint) (int, error) {
	str := fmt.Sprintf("<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" width=\"%d\" height=\"%d\">\n", width, height)
	return file.WriteString(str)
}

func writeFooter(file *os.File) (int, error) {
	return file.WriteString("</svg>\n")
}

func svgLine(file *os.File, x1 float64, y1 float64, x2 float64, y2 float64, stroke string) (int, error) {
	str := fmt.Sprintf("\t<line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\" stroke=\"%s\" style=\"stroke-width:0.5;\"/>\n", x1, y1, x2, y2, stroke)
	return file.WriteString(str)
}

func svgRect(file *os.File, x float64, y float64, w float64, h float64, stroke string, fill string) (int, error) {
	str := fmt.Sprintf("\t<rect x=\"%f\" y=\"%f\" width=\"%f\" height=\"%f\" fill=\"%s\" stroke=\"%s\" style=\"stroke-width:0.25;\"/>\n", x, y, w, h, fill, stroke)
	return file.WriteString(str)
}

func (m *Maze) SaveSVG(name string) error {
	file, err := os.Create(name)
	if err != nil {
		return JoinError("error opening file", err)
	}
	defer file.Close()

	width := m.tree.width + 6
	height := m.tree.height + 6
	_, err = writeHeader(file, width, height)
	if err != nil {
		return JoinError("error writing header", err)
	}

	_, err = svgRect(file, 0, 0, float64(width), float64(height), SVG_NONE_COLOR, SVG_BACK_COLOR)
	if err != nil {
		return JoinError("error writing outline", err)
	}
	for i := 0; i < int(m.rooms); i++ {
		room := m.array[i]
		_, err = svgRect(file, float64(room.x)+3.0, float64(room.y)+3.0, float64(room.width), float64(room.height), SVG_WALL_COLOR, SVG_NONE_COLOR)
		if err != nil {
			return JoinError(fmt.Sprintf("error writing room #%d", room.findRoomIndex(m.array)), err)
		}
	}
	for i := 0; i < int(m.rooms); i++ {
		room := m.array[i]
		if room.door_x == 0 && room.door_y == 0 {
			continue
		}
		if room.height > room.width {
			_, err = svgLine(file, float64(room.door_x)+2.9, float64(room.door_y)+3.0, float64(room.door_x)+2.1, float64(room.door_y)+3.0, SVG_DOOR_COLOR)
		} else {
			_, err = svgLine(file, float64(room.door_x)+3.0, float64(room.door_y)+2.9, float64(room.door_x)+3.0, float64(room.door_y)+2.1, SVG_DOOR_COLOR)
		}
		if err != nil {
			return JoinError(fmt.Sprintf("error writing door #%d", room.findRoomIndex(m.array)), err)
		}
	}

	_, err = writeFooter(file)
	if err != nil {
		return JoinError("error writing footer", err)
	}
	return nil
}
