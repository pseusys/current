package classes;

class Square {
    private int x, y, size;
    private boolean xVector, yVector;

    Square(int x, int y, int size, boolean xVector, boolean yVector) {
        this.x = x;
        this.y = y;
        this.size = size;
        this.xVector = xVector;
        this.yVector = yVector;
    }

    int getX() {
        return x;
    }

    void setX(int x) {
        this.x = x;
    }

    int getY() {
        return y;
    }

    void setY(int y) {
        this.y = y;
    }

    int getSize() {
        return size;
    }

    void setSize(int size) {
        this.size = size;
    }

    boolean getXDirection() {
        return xVector;
    }

    boolean getYDirection() {
        return yVector;
    }

    Square turn(boolean xVec, boolean yVec) {
        if (xVec != xVector) x += xVector ? size - 1 : -size + 1;
        if (yVec != yVector) y += yVector ? size - 1 : -size + 1;
        xVector = xVec;
        yVector = yVec;
        return this;
    }

    Square reflect() {
        int prevX = x;
        x = -y;
        y = -prevX;
        yVector = !yVector;
        xVector = !xVector;
        return this;
    }

    Square subSquare(int xOffset, int yOffset, int subSize) {
        int subX = x + (xVector ? xOffset : -xOffset);
        int subY = y + (yVector ? yOffset : -yOffset);
        return new Square(subX, subY, subSize, xVector, yVector);
    }

    Square copy() {
        return new Square(x, y, size, xVector, yVector);
    }

    @Override
    public String toString() {
        return "Square{" +
                "x=" + x +
                ", y=" + y +
                ", size=" + size +
                '}';
    }
}
