package superclass;

import java.util.LinkedList;
import java.util.List;
import java.util.Scanner;

public class Superclass {
    public static void main(String[] args) {
        //Scanner sc = new Scanner(System.in);
        //int size = sc.nextInt();
        for (int i = 2; i < 21; i++) {
            FieldCoverage fc = new FieldCoverage(i);
            List<Square> out = fc.fillEmptySquareNormally();
            System.out.println(i + " : " + out.size());
            /*for (Square sq: out) {
                System.out.println(sq);
            }*/
        }
    }
}



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
        return x + " " + y + " " + size;
    }
}



class PivotedSquare extends Square {
    public enum Cut {NOT_CUT, CUT, DOUBLE_CUT, UNKNOWN}
    private Cut isCut;

    PivotedSquare(int x, int y, int size, boolean xVector, boolean yVector, Cut isCut) {
        super(x, y, size, xVector, yVector);
        this.isCut = isCut;
    }

    private PivotedSquare(Square src, Cut isCut) {
        this(src.getX(), src.getY(), src.getSize(), src.getXDirection(), src.getYDirection(), isCut);
    }

    Cut isCut() {
        return isCut;
    }



    void fillSquare(LinkedList<Square> fills, LinkedList<PivotedSquare> pivots) {
        pivots.removeFirst();
        int halfSize = getSize() / 2;

        if (isCut != Cut.DOUBLE_CUT) {
            if (getSize() % 2 == 0) {
                fills.addLast(subSquare(0, 0, halfSize).turn(true, true));
                fills.addLast(subSquare(halfSize, 0, halfSize).turn(true, true));
                fills.addLast(subSquare(0, halfSize, halfSize).turn(true, true));

                Cut cut = Cut.UNKNOWN;
                switch (isCut) {
                    case CUT:
                        cut = Cut.CUT;
                        break;
                    case NOT_CUT:
                        cut = Cut.NOT_CUT;
                        break;
                }
                if ((cut == Cut.NOT_CUT) || (halfSize != 1)) pivots.addLast(new PivotedSquare(subSquare(halfSize, halfSize, halfSize), cut));
            } else {
                fills.addLast(subSquare(0, 0, halfSize + 1).turn(true, true));
                fills.addLast(subSquare(halfSize + 1, 0, halfSize).turn(true, true));
                fills.addLast(subSquare(0, halfSize + 1, halfSize).turn(true, true));

                Cut cut = Cut.UNKNOWN;
                boolean newXVector = true, newYVector = true;
                switch (isCut) {
                    case CUT:
                        cut = Cut.DOUBLE_CUT;
                        newXVector = !getXDirection();
                        newYVector = getYDirection();
                        break;
                    case NOT_CUT:
                        cut = Cut.CUT;
                        newXVector = !getXDirection();
                        newYVector = !getYDirection();
                        break;
                }
                pivots.addLast(new PivotedSquare(subSquare(halfSize, halfSize, halfSize + 1).turn(newXVector, newYVector), cut));
            }

        } else {
            LinkedList<Square> leftLower = new LinkedList<>();
            LinkedList<Square> rightUpper = new LinkedList<>();

            int squareCountMinimum = getSize();
            int smallerSquareSize = 1;
            LinkedList<Square> temp = new LinkedList<>();
            for (int i = 1; i <= halfSize; i++) {
                Bus bus = new Bus(i, getSize() - i);
                temp = bus.count();
                if (temp.size() <= squareCountMinimum) {
                    leftLower = temp;
                    squareCountMinimum = leftLower.size();
                    smallerSquareSize = i;
                } else break;
            }

            int biggerSquareSize = getSize() - smallerSquareSize;

            fills.add(subSquare(0, 0, biggerSquareSize).turn(true, true));
            fills.add(subSquare(biggerSquareSize, biggerSquareSize, smallerSquareSize).turn(true, true));

            int succX = getX() + biggerSquareSize;
            int succY = getY() - biggerSquareSize;

            for (Square sq : leftLower) {
                rightUpper.addLast(sq.copy());
            }
            for (Square sq : rightUpper) {
                sq.reflect().turn(true, true);
                sq.setX(sq.getX() + succX - 1);
                sq.setY(sq.getY() + succY);
                fills.add(sq);
            }

            for (Square sq : leftLower) {
                sq.setX(sq.getX() + succX);
                sq.setY(sq.getY() + succY + 1);
                fills.add(sq);
            }
        }
    }
}



class Bus {
    private int width, height;
    private boolean vertical;
    private LinkedList<Square> contains;

    Bus(int width, int height) {
        this.width = width;
        this.height = height;
        contains = new LinkedList<>();
        vertical = height > width;
    }

    LinkedList<Square> count() {
        boolean mutVert = vertical;
        int mutWidth = width;
        int mutHeight = height;

        while (mutWidth != mutHeight) {
            Square novus;
            if (mutVert) {
                novus = new Square(width - mutWidth, height - mutHeight, mutWidth, true, true);
                mutHeight -= mutWidth;
            } else {
                novus = new Square( width - mutWidth, height - mutHeight, mutHeight, true, true);
                mutWidth -= mutHeight;
            }

            contains.add(novus);
            mutVert = mutHeight > mutWidth;
        }

        if ((mutWidth == 1) && (mutHeight == 1)) return contains;

        superclass.FieldCoverage fc = new superclass.FieldCoverage(mutWidth, PivotedSquare.Cut.CUT);
        for (Square sq: fc.fillEmptySquareNormally()) {
            sq.setX(sq.getX() + (width - mutWidth));
            sq.setY(sq.getY() + (height - mutHeight));
            contains.add(sq);
        }
        return contains;
    }
}



class FieldCoverage {
    private static final int [] smpl = {97, 89, 84, 79, 73, 71, 67, 61, 59, 53, 47, 43, 41, 37, 31, 29, 23, 19, 17, 13, 11, 7, 5, 3, 2};

    private LinkedList<Square> insideSquares;
    private LinkedList<PivotedSquare> emptySpacePivots;
    private int multiplier;

    FieldCoverage(int size) {
        this(size, PivotedSquare.Cut.NOT_CUT);
    }

    FieldCoverage(int size, PivotedSquare.Cut cut) {
        this(size, cut, true, true);
    }

    private FieldCoverage(int size, PivotedSquare.Cut cut, boolean xv, boolean yv) {
        multiplier = 1;
        insideSquares = new LinkedList<>();
        emptySpacePivots = new LinkedList<>();
        emptySpacePivots.addLast(new PivotedSquare(0, 0, size, xv, yv, cut));
    }

    List<Square> fillEmptySquareNormally() {
        if (emptySpacePivots.getLast().isCut() == PivotedSquare.Cut.NOT_CUT) {
            for (int i = 0; i < smpl.length; i++)
                while ((emptySpacePivots.getLast().getSize() % smpl[i] == 0) && (emptySpacePivots.getLast().getSize() / smpl[i] != 1)) {
                    emptySpacePivots.getLast().setSize(emptySpacePivots.getLast().getSize() / smpl[i]);
                    multiplier *= smpl[i];
                }

            if (emptySpacePivots.getLast().getSize() == 2) {
                emptySpacePivots.getLast().fillSquare(insideSquares, emptySpacePivots);
                insideSquares.addLast(emptySpacePivots.getLast());
                emptySpacePivots.clear();
            }
        }

        while (!emptySpacePivots.isEmpty()) emptySpacePivots.getLast().fillSquare(insideSquares, emptySpacePivots);

        if (multiplier != 1) for (Square sq : insideSquares) {
            sq.setSize(sq.getSize() * multiplier);
            sq.setX(sq.getX() * multiplier);
            sq.setY(sq.getY() * multiplier);
        }

        return insideSquares;
    }
}

