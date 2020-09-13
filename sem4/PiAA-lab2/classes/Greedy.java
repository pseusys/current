package com.company;

import java.util.*;

public class Greedy extends Pathfinder {
    @Override
    protected Path find(char start, char end) {
        out.print(start);
        if (start == end) return new Path(start, 0);
        else out.print(" -> ");

        if (nodes.get(start).isEmpty()) return null;

        double shortestLength = Collections.min(nodes.get(start).values());
        LinkedList<Path> paths = new LinkedList<>();
        for (Map.Entry<Character, Double> map : nodes.get(start).entrySet()) {
            if (map.getValue() == shortestLength) {
                Path path = find(map.getKey(), end);
                if (path != null) {
                    paths.add(path.addFront(start, shortestLength));
                }
            }
        }

        if (paths.isEmpty()) return null;

        Path SP = paths.peek();
        double shortestPath = SP.getLength();
        for (Path path : paths) if (path.getLength() < shortestPath) {
            shortestPath = path.getLength();
            SP = path;
        }

        return SP;
    }
}
