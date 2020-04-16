package com.company;

import java.util.LinkedList;

public class TableCoverage {
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
