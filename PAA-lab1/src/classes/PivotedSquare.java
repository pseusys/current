package classes;

import java.util.LinkedList;

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
