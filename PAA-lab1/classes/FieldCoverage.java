package classes;

import java.util.Arrays;
import java.util.LinkedList;
import java.util.List;

public class FieldCoverage {
    LinkedList<Square> insideSquares;
    LinkedList<PivotedSquare> emptySpacePivots;

    public FieldCoverage(int size) {
        this(size, PivotedSquare.Cut.NOT_CUT);
    }

    public FieldCoverage(int size, PivotedSquare.Cut cut) {
        insideSquares = new LinkedList<>();
        emptySpacePivots = new LinkedList<>();
        emptySpacePivots.addLast(new PivotedSquare(0, 0, size, true, true, cut));
    }

    List<Square> fillEmptySquareNormally() {
        while (!emptySpacePivots.isEmpty()) {
            emptySpacePivots.getLast().fillSquare(insideSquares, emptySpacePivots);
            //System.out.println(Arrays.toString(insideSquares.toArray()) + " : " + Arrays.toString(emptySpacePivots.toArray()));
            if ((!emptySpacePivots.isEmpty()) && (emptySpacePivots.getLast().isCut() == PivotedSquare.Cut.NOT_CUT)) {
                insideSquares.addLast(emptySpacePivots.getLast());
                break;
            }
        }
        return insideSquares;
    }
}
