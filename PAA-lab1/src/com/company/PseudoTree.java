package com.company;


import java.util.Arrays;
import java.util.Collections;
import java.util.LinkedList;
import java.util.Stack;

public class PseudoTree {
    public static final int [] smp = new int [] {97, 89, 83, 79, 73, 71, 67, 61, 59, 53, 47, 43, 41, 37, 31, 29, 23, 19, 17, 13, 11, 7, 5, 3, 2};

    private int multiplier = 1;
    private LinkedList<Square> head;
    private TableCoverage root;
    private boolean hasTail;
    private int tailOffset;

    public int leavesNumber = 0;

    public PseudoTree(int sz) {
        for (int i = 0; i < smp.length; i++) {
            if ((sz % smp[i] == 0) && (sz / smp[i] != 1)) {
                sz /= smp[i];
                multiplier *= smp[i];
                i--;
            }
        }

        head = new LinkedList<>();
        int halfSize = sz / 2 + (sz == 2 ? 0 : 1);

        head.add(new Square(0, 0, halfSize));
        head.add(new Square(halfSize, 0, sz / 2));
        head.add(new Square(0, halfSize, sz / 2));

        if (sz == 2) {
            head.add(new Square(halfSize, halfSize, halfSize));
            hasTail = false;
        } else {
            tailOffset = halfSize;
            root = new TableCoverage(halfSize);
            hasTail = true;
        }
    }



    public LinkedList<Square> buildAndParseTree() {
        LinkedList<Square> answer = new LinkedList<>();
        for (Square sq : head) answer.push(sq);

        if (hasTail) {
            Stack<Square> tail = backtrackRows();
            //Stack<Square> tail = getIteration();

            for (Square sq : tail) {
                sq.setX(2*tailOffset - sq.getSize() - sq.getX() - 1);
                sq.setY(2*tailOffset - sq.getSize() - sq.getY() - 1);
            }

            answer.addAll(tail);
        }

        for (Square sq : answer) {
            sq.setX(1 + sq.getX() * multiplier);
            sq.setY(1 + sq.getY() * multiplier);
            sq.setSize(sq.getSize() * multiplier);
        }

        return answer;
    }

    private Stack<Square> backtrackRows() {
        Stack<Square> filling = new Stack<>();
        int maxSize = root.getSize() * root.getSize();
        Stack<Square> idealFilling = new Stack<>();

        while (maxSize > 1) {
            Square novus = root.addSquare();
            root.setSize(maxSize);

            while (novus != null) {
                filling.push(novus);
                root.cover(novus);
                if ((!idealFilling.isEmpty()) && (filling.size() > idealFilling.size())) break;
                novus = root.addSquare();
            }

            if ((filling.size() < idealFilling.size()) || (idealFilling.isEmpty())) {
                idealFilling.clear();
                idealFilling.addAll(filling);
            }

            Square top;
            maxSize = 1;
            while (!filling.isEmpty()) {
                top = filling.pop();
                maxSize = top.getSize();
                root.uncover(top);
                if (maxSize > 1) {
                    root.setSize(maxSize - 1);
                    break;
                }
            }
        }

        return idealFilling;
    }

    private Stack<Square> getIteration() {
        TableCoverage complete = iterateRowsUntilSuccess();
        Stack<Square> tail = new Stack<>();

        while (complete.getParent() != null) {
            tail.push(complete.getPayload());
            complete = complete.getParent();
        }

        return tail;
    }

    private TableCoverage iterateRowsUntilSuccess() {
        LinkedList<TableCoverage> currentRow = new LinkedList<>(Collections.singletonList(root));
        LinkedList<TableCoverage> newRow = new LinkedList<>();
        TableCoverage finalContainer = null;

        while (finalContainer == null) {
            for (TableCoverage leaf : currentRow) {
                LinkedList<Square> children = leaf.addSquares();
                if (children.isEmpty()) {
                    finalContainer = leaf;
                    break;
                } else leavesNumber += children.size();
                for (Square square : children) newRow.add(new TableCoverage(leaf, square));
            }
            currentRow = newRow;
            newRow = new LinkedList<>();
        }

        return finalContainer;
    }


    public static String checkList(LinkedList<Square> squares, int size) {
        char [][] form = new char [size][size];

        for (int k = 0; k < squares.size(); k++) {
            for (int i = squares.get(k).getY() - 1; i < squares.get(k).getY() - 1 + squares.get(k).getSize(); i++) {
                for (int j = squares.get(k).getX() - 1; j < squares.get(k).getX() - 1 + squares.get(k).getSize(); j++) {
                    form[i][j] = (char) (k + 'A');
                }
            }
        }

        StringBuilder sb = new StringBuilder();
        for (char [] chars : form) {
            sb.append(Arrays.toString(chars)).append("\n");
        }
        return sb.toString();
    }
}
