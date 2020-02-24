package classes;

import java.util.LinkedList;
import java.util.List;

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
