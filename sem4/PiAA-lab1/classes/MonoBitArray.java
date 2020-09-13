package com.company;

public class MonoBitArray {
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
