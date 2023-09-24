package main

import (
	"container/list"
	"errors"
	"fmt"
	"os"
)

const (
	PPM_INTENSITY = 255
)

var (
	PPM_BACK_COLOR = [3]byte{220, 220, 220}
	PPM_WALL_COLOR = [3]byte{56, 56, 56}
	PPM_DOOR_COLOR = [3]byte{222, 184, 135}
	PPM_PATH_COLOR = [3]byte{220, 28, 28}
	PPM_WIDTH      uint
	PPM_HEIGHT     uint
)

func fillBuffer(color [3]byte, buffer []byte, length int) {
	for i := 0; i < length; i++ {
		for j := 0; j < 3; j++ {
			buffer[i*3+j] = color[j]
		}
	}
}

func ppmHeader(file *os.File) (int, error) {
	str := fmt.Sprintf("P6\n%d %d\n%d\n", PPM_WIDTH, PPM_HEIGHT, PPM_INTENSITY)
	return file.WriteString(str)
}

func ppmBuffer(file *os.File, buffer []byte) (int, error) {
	return file.Write(buffer)
}

func ppmLine(color [3]byte, buffer []byte, x1 uint, y1 uint, x2 uint, y2 uint, shrink uint) error {
	var line_start, line_end, line_coord uint
	if x1 == x2 {
		line_coord = x1 + 3
		if y1 > y2 {
			line_start = y2 + 3 + shrink
			line_end = y1 + 3 - shrink
		} else {
			line_start = y1 + 3 + shrink
			line_end = y2 + 3 - shrink
		}
		for i := line_start; i <= line_end; i++ {
			for j := 0; j < 3; j++ {
				buffer[(i*PPM_WIDTH+line_coord)*3+uint(j)] = color[j]
			}
		}
	} else if y1 == y2 {
		line_coord = y1 + 3
		if x1 > x2 {
			line_start = x2 + 3 + shrink
			line_end = x1 + 3 - shrink
		} else {
			line_start = x1 + 3 + shrink
			line_end = x2 + 3 - shrink
		}
		for i := line_start; i <= line_end; i++ {
			for j := 0; j < 3; j++ {
				buffer[(line_coord*PPM_WIDTH+i)*3+uint(j)] = color[j]
			}
		}
	} else {
		return errors.New("walls should be either vertical or horizontal")
	}
	return nil
}

func ppmMerge(color [3]byte, buffer []byte, start *Room, end *Room) (err error) {
	if start.isFinal() == end.isFinal() {
		return errors.New("can't merge rooms of the same type")
	}
	var x1, y1, x2, y2, x3, y3 uint
	if start.isFinal() {
		x1 = start.x*4 + start.width*2
		y1 = start.y*4 + start.height*2
		if end.doorHorizontal() {
			x2 = end.door_x*4 + 2
			y2 = y1
		} else {
			x2 = x1
			y2 = end.door_y*4 + 2
		}
		x3 = end.door_x*4 + 2
		y3 = end.door_y*4 + 2
	} else {
		return ppmMerge(color, buffer, end, start)
	}
	LogF("%d, %d -> %d, %d -> %d, %d\n", x1, y1, x2, y2, x3, y3)
	err = ppmLine(color, buffer, x1, y1, x2, y2, 0)
	if err != nil {
		return JoinError("error writing first line", err)
	}
	err = ppmLine(color, buffer, x2, y2, x3, y3, 0)
	if err != nil {
		return JoinError("error writing second line", err)
	}
	return nil
}

func (r *Room) ppmWall(buffer []byte) error {
	if r.doorHorizontal() {
		return ppmLine(PPM_WALL_COLOR, buffer, r.x*4, r.door_y*4, (r.x+r.width)*4, r.door_y*4, 0)
	} else {
		return ppmLine(PPM_WALL_COLOR, buffer, r.door_x*4, r.y*4, r.door_x*4, (r.y+r.height)*4, 0)
	}
}

func (r *Room) ppmDoor(buffer []byte) error {
	if r.doorHorizontal() {
		return ppmLine(PPM_DOOR_COLOR, buffer, r.door_x*4, r.door_y*4, (r.door_x+1)*4, r.door_y*4, 1)
	} else {
		return ppmLine(PPM_DOOR_COLOR, buffer, r.door_x*4, r.door_y*4, r.door_x*4, (r.door_y+1)*4, 1)
	}
}

func (r *Room) ppmOutline(buffer []byte) (err error) {
	err = ppmLine(PPM_WALL_COLOR, buffer, r.x*4, r.y*4, (r.x+r.width)*4, r.y*4, 0)
	if err != nil {
		return err
	}
	err = ppmLine(PPM_WALL_COLOR, buffer, r.x*4, (r.y+r.height)*4, (r.x+r.width)*4, (r.y+r.height)*4, 0)
	if err != nil {
		return err
	}
	err = ppmLine(PPM_WALL_COLOR, buffer, r.x*4, r.y*4, r.x*4, (r.y+r.height)*4, 0)
	if err != nil {
		return err
	}
	err = ppmLine(PPM_WALL_COLOR, buffer, (r.x+r.width)*4, r.y*4, (r.x+r.width)*4, (r.y+r.height)*4, 0)
	if err != nil {
		return err
	}
	return nil
}

func (m *Maze) SavePPM(name string, path *list.List) error {
	file, err := os.Create(name)
	if err != nil {
		return JoinError("error opening file", err)
	}
	defer file.Close()

	PPM_WIDTH = m.tree.width*4 + 7
	PPM_HEIGHT = m.tree.height*4 + 7
	_, err = ppmHeader(file)
	if err != nil {
		return JoinError("error writing header", err)
	}
	buffer_size := PPM_WIDTH * PPM_HEIGHT
	buffer := make([]byte, buffer_size*3)

	fillBuffer(PPM_BACK_COLOR, buffer, int(buffer_size))
	m.tree.ppmOutline(buffer)

	for i := 0; i < int(m.rooms); i++ {
		room := m.array[i]
		if room.isFinal() {
			continue
		}
		err = room.ppmWall(buffer)
		if err != nil {
			return JoinError(fmt.Sprintf("error writing room #%d", room.findRoomIndex(m.array)), err)
		}
		err = room.ppmDoor(buffer)
		if err != nil {
			return JoinError(fmt.Sprintf("error writing door #%d", room.findRoomIndex(m.array)), err)
		}
	}

	if path.Len() > 1 {
		var previous *Room
		for e := path.Front(); e != nil; e = e.Next() {
			current := e.Value.(*Room)
			if previous != nil {
				err = ppmMerge(PPM_PATH_COLOR, buffer, current, previous)
			}
			if err != nil {
				return JoinError("error writing path", err)
			}
			previous = current
		}
	}

	_, err = ppmBuffer(file, buffer)
	if err != nil {
		return JoinError("error writing buffer", err)
	}
	return nil
}
