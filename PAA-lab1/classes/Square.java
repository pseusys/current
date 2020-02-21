package classes;

public class Square {
    protected int x, y, size;
    protected boolean xVector, yVector;

    public Square(int x, int y, int size, boolean xVector, boolean yVector) {
        this.x = x;
        this.y = y;
        this.size = size;
        this.xVector = xVector;
        this.yVector = yVector;
    }

    public int getX() {
        return x;
    }

    public int getY() {
        return y;
    }

    public int getSize() {
        return size;
    }

    public boolean isEmpty() {
        return size == 0;
    }

    public boolean getXDirection() {
        return xVector;
    }

    public boolean getYDirection() {
        return yVector;
    }

    Square turn(boolean xVec, boolean yVec) {
        if (xVec != xVector) x += xVector ? size - 1 : -size + 1;
        if (yVec != yVector) y += yVector ? size - 1 : -size + 1;
        xVector = xVec;
        yVector = yVec;
        return this;
    }

    Square subSquare(int xOffset, int yOffset, int subSize) {
        int subX = x + (xVector ? xOffset : -xOffset);
        int subY = y + (yVector ? yOffset : -yOffset);
        return new Square(subX, subY, subSize, xVector, yVector);
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
