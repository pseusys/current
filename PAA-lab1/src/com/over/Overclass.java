package com.over;

import java.util.*;

public class Overclass {
    private static class Coords {
        private int x, y;

        public Coords(int x, int y) {
            this.x = x;
            this.y = y;
        }

        public int getX() {
            return x;
        }

        public int getY() {
            return y;
        }

        public void setX(int x) {
            this.x = x;
        }

        public void setY(int y) {
            this.y = y;
        }
    }

    private static class Square extends Overclass.Coords {
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

    private static class MonoBitArray {
        private int [] data;

        public MonoBitArray(int size) {
            data = new int [size];
        }

        public Coords findFirstEmpty() {
            int full = Integer.MAX_VALUE % (1 << data.length);

            for (int i = 0; i < data.length; i++) {
                int partial = data[i] % (1 << data.length);
                if (partial < full) {
                    int counter = 0, diff = full - partial;
                    while (diff > 1) {
                        diff >>= 1;
                        counter++;
                    }
                    return new Coords(data.length - 1 - counter, i);
                }
            }

            return null;
        }

        public boolean isOccupied(Coords coords) {
            int k = data[coords.getY()];
            int l = (data.length - 1 - coords.getX());
            return (k >> l) % 2 != 0;
        }

        public int getLine(int num) {
            return data[num];
        }

        public void addLine(int num, int line) {
            data[num] += line;
        }

        public void deleteLine(int num, int line) {
            data[num] -= line;
        }



        int size() {
            return data.length;
        }

        @Override
        public String toString() {
            StringBuilder output = new StringBuilder();
            for (int line : data) {
                output.append(String.format("%" + data.length + "s", Integer.toBinaryString(line)).replace(' ', '0'));
                output.append('\n');
            }
            return output.toString();
        }
    }



    private static class TableCoverage {
        private TableCoverage parent;

        private MonoBitArray table;
        private Square lastAdded;
        private int size;

        public TableCoverage(int sz) {
            parent = null;

            table = new MonoBitArray(sz);
            table.addLine(sz - 1, 1);
            size = sz / 2 + 1;
        }

        public TableCoverage(TableCoverage parental, Square additional) {
            parent = parental;

            table = new MonoBitArray(parental.table.size());
            for (int i = 0; i < parental.table.size(); i++) {
                table.addLine(i, parental.table.getLine(i));
            }
            lastAdded = additional;
            size = parental.size;

            for (int i = additional.getY(); i < additional.getY() + additional.getSize(); i++) {
                for (int j = additional.getX(); j < additional.getX() + additional.getSize(); j++) {
                    table.addLine(i, 1 << (table.size() - 1 - j));
                }
            }
        }

        public void cover(Square additional) {
            for (int i = additional.getY(); i < additional.getY() + additional.getSize(); i++) {
                for (int j = additional.getX(); j < additional.getX() + additional.getSize(); j++) {
                    table.addLine(i, 1 << (table.size() - 1 - j));
                }
            }
        }

        public void uncover(Square additional) {
            for (int i = additional.getY(); i < additional.getY() + additional.getSize(); i++) {
                for (int j = additional.getX(); j < additional.getX() + additional.getSize(); j++) {
                    table.deleteLine(i, 1 << (table.size() - 1 - j));
                }
            }
        }

        public void setSize(int size) {
            this.size = size;
        }

        public int getSize() {
            return size;
        }

        public TableCoverage getParent() {
            return parent;
        }

        public Square getPayload() {
            return lastAdded;
        }



        public LinkedList<Square> addSquares() {
            LinkedList<Square> tp = new LinkedList<>();

            Coords pos = table.findFirstEmpty();
            if (pos == null) return tp;

            int maxY = Math.min(table.size(), pos.getY() + size);
            int maxX = Math.min(table.size(), pos.getX() + size);
            int occupiedX = maxX, occupiedY = maxY, topSize;

            for (int k = pos.getY() + 1; k < maxY; k++) {
                if (table.isOccupied(new Coords(pos.getX(), k))) {
                    occupiedY = k;
                    break;
                }
            }
            for (int l = pos.getX() + 1; l < maxX; l++) {
                if (table.isOccupied(new Coords(l, pos.getY()))) {
                    occupiedX = l;
                    break;
                }
            }
            topSize = Math.min(occupiedX - pos.getX(), occupiedY - pos.getY());
            if (table.isOccupied(new Coords(pos.getX() + topSize - 1, pos.getY() + topSize - 1))) topSize--;

            for (int k = 1; k <= topSize; k++) tp.push(new Square(pos.getX(), pos.getY(), k));

            return tp;
        }

        public Square addSquare() {
            LinkedList<Square> added = addSquares();
            return added.isEmpty() ? null : added.pop();
        }



        @Override
        public String toString() {
            return table.toString();
        }
    }

    private static class PseudoTree {
        public static final int [] smp = new int [] {97, 89, 83, 79, 73, 71, 67, 61, 59, 53, 47, 43, 41, 37, 31, 29, 23, 19, 17, 13, 11, 7, 5, 3, 2};

        private int multiplier = 1;
        private LinkedList<Square> head;
        private TableCoverage root;
        private boolean hasTail;
        private int tailOffset;
        private int halfSize;

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
            halfSize = sz / 2 + (sz == 2 ? 0 : 1);

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



        public LinkedList<Square> buildAndParseTree(boolean useTree) {
            LinkedList<Square> answer = new LinkedList<>();
            for (Square sq : head) answer.push(sq);

            if (hasTail) {
                Stack<Square> tail;
                if (useTree) tail = getIteration();
                else tail = backtrackRows();

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
            int maxSize = halfSize * halfSize;
            Stack<Square> idealFilling = new Stack<>();

            while (maxSize > 1) {
                Square novus = root.addSquare();
                root.setSize(halfSize);

                while (novus != null) {
                    filling.push(novus);
                    root.cover(novus);
                    if ((!idealFilling.isEmpty()) && (filling.size() > idealFilling.size())) break;
                    novus = root.addSquare();
                    leavesNumber++;
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


    public static void main(String[] args) {
        /*Scanner sc = new Scanner(System.in);
        int size = sc.nextInt();

        Overclass.PseudoTree tree = new Overclass.PseudoTree(size);
        LinkedList<Overclass.Square> ans = tree.buildAndParseTree(false);

        System.out.println(ans.size());
        for (Overclass.Square sq : ans) {
            System.out.println(sq.toString());
        }*/

        for (int i = 2; i < 45; i++) {
            Overclass.PseudoTree tree = new Overclass.PseudoTree(i);
            LinkedList<Overclass.Square> ans = tree.buildAndParseTree(false);
            System.out.println(ans.size());
            for (Overclass.Square sq : ans) {
                System.out.println(sq.toString());
            }
            System.out.println();
        }
    }
}
