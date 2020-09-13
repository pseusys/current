package com.company;

public class Square extends Coords {
    private int size;

    public Square(int x, int y, int size) {
        super(x, y);
        this.size = size;
    }

    public int getSize() {
        return size;
    }

    public void setSize(int size) {
        this.size = size;
    }

    @Override
    public String toString() {
        return getX() + " " + getY() + " " + size;
    }
}
