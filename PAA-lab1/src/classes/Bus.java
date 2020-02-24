package classes;

import java.util.LinkedList;

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

        FieldCoverage fc = new FieldCoverage(mutWidth, PivotedSquare.Cut.CUT);
        for (Square sq: fc.fillEmptySquareNormally()) {
            sq.setX(sq.getX() + (width - mutWidth));
            sq.setY(sq.getY() + (height - mutHeight));
            contains.add(sq);
        }
        return contains;
    }
}
