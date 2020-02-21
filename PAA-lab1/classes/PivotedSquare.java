package classes;

import java.util.LinkedList;

public class PivotedSquare extends Square {
    public enum Cut {NOT_CUT, CUT, DOUBLE_CUT, UNKNOWN}
    private Cut isCut;

    public PivotedSquare(int x, int y, int size, boolean xVector, boolean yVector, Cut isCut) {
        super(x, y, size, xVector, yVector);
        this.isCut = isCut;
    }

    public PivotedSquare(Square src, Cut isCut) {
        this(src.x, src.y, src.size, src.xVector, src.yVector, isCut);
    }

    public Cut isCut() {
        return isCut;
    }

    public void setCut(Cut isCut) {
        this.isCut = isCut;
    }





    public void fillSquare(LinkedList<Square> fills, LinkedList<PivotedSquare> pivots) {
        pivots.removeFirst();
        if (isCut != Cut.DOUBLE_CUT) {
            if (size % 2 == 0) {
                fills.addLast(subSquare(0, 0, size / 2));
                fills.addLast(subSquare(size / 2, 0, size / 2));
                fills.addLast(subSquare(0, size / 2, size / 2));

                Cut cut = Cut.UNKNOWN;
                switch (isCut) {
                    case CUT:
                        cut = Cut.CUT;
                        break;
                    case NOT_CUT:
                        cut = Cut.NOT_CUT;
                        break;
                }
                if ((cut == Cut.NOT_CUT) || (size / 2 != 1)) pivots.addLast(new PivotedSquare(subSquare(size / 2, size / 2, size / 2), cut));
            } else {
                fills.addLast(subSquare(0, 0, size / 2 + 1));
                fills.addLast(subSquare(size / 2 + 1, 0, size / 2));
                fills.addLast(subSquare(0, size / 2 + 1, size / 2));

                Cut cut = Cut.UNKNOWN;
                boolean newXVector = true, newYVector = true;
                switch (isCut) {
                    case CUT:
                        cut = Cut.DOUBLE_CUT;
                        newXVector = !xVector;
                        newYVector = yVector;
                        break;
                    case NOT_CUT:
                        cut = Cut.CUT;
                        newXVector = !xVector;
                        newYVector = !yVector;
                        break;
                }
                pivots.addLast(new PivotedSquare(subSquare(size / 2, size / 2, size / 2 + 1).turn(newXVector, newYVector), cut));
            }
        } else {
            int minimal = size - 2;
            int trueSize = 0;
            for (int i = size / 2; i <= size - 1; i++) {
                int controlSum = (size - 1) / i - 1;
                FieldCoverage fc = new FieldCoverage((size - 1) % i + 1, Cut.CUT);
                controlSum += fc.fillEmptySquareNormally().size();
                if (controlSum < minimal) {
                    minimal = controlSum;
                    trueSize = i;
                }
            }

            //TODO: recalculate with given size;
        }
    }
}
