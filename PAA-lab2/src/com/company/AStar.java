package com.company;

import java.util.LinkedList;
import java.util.Map;
import java.util.TreeMap;

public class AStar extends Pathfinder {
    private char first, last;

    private double g(Path path) {
        return path.getLength();
    }

    private double h(Path path) {
        return Math.abs((int) last - (int) path.getEnd());
    }

    private double f(Path path) {
        return g(path) + h(path);
    }

    @Override
    public Path find(char first, char last) {
        this.first = first;
        this.last = last;

        LinkedList<Path> closed = new LinkedList<>();
        TreeMap<Double, Path> opened = new TreeMap<>();

        Path beginning = new Path(first, 0);
        opened.put(f(beginning), beginning);
        out.print("Checking: " + beginning.getEnd());

        while (!opened.isEmpty()) {
            Map.Entry<Double, Path> current = opened.firstEntry();

            if (current.getValue().getEnd() == last) return current.getValue();

            opened.remove(current.getKey());
            closed.push(current.getValue());

            for (Map.Entry<Character, Double> near: nodes.get(current.getValue().getEnd()).entrySet()) {
                Path vertex = new Path(current.getValue().getLiteral(), current.getValue().getLength())
                        .addBack(near.getKey(), near.getValue());

                if (!contains(closed, vertex)) {
                    out.print("\nChecking: " + vertex.getEnd());
                    if (contains(opened, vertex)) {
                        double prevDist = getDistance(opened, vertex);
                        if (prevDist > f(vertex)) {
                            opened.remove(prevDist);
                            opened.put(f(vertex), vertex);
                        }
                    } else {
                        opened.put(f(vertex), vertex);
                    }
                }
            }
        }

        return null;
    }

    private boolean contains(LinkedList<Path> paths, Path path) {
        for (Path p : paths) {
            if (p.getEnd() == path.getEnd()) return true;
        }
        return false;
    }

    private boolean contains(TreeMap<Double, Path> paths, Path path) {
        for (Path p : paths.values()) {
            if (p.getEnd() == path.getEnd()) return true;
        }
        return false;
    }

    private double getDistance(TreeMap<Double, Path> paths, Path path) {
        for (Map.Entry<Double, Path> p : paths.entrySet()) {
            if (p.getValue().getEnd() == path.getEnd()) return p.getKey();
        }
        return -1;
    }
}
