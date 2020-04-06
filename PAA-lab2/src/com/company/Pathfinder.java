package com.company;

import java.util.HashMap;
import java.util.Scanner;

public abstract class Pathfinder {
    HashMap<Character, HashMap<Character, Double>> nodes = new HashMap<>();

    public String solve() {
        Scanner sc = new Scanner(System.in);
        char first = sc.next().charAt(0);
        char last = sc.next().charAt(0);

        int len = (int) last - (int) first;
        nodes = new HashMap<>(len);

        char source;
        char target;
        double weight;
        while (sc.hasNextLine()) {
            source = sc.next().charAt(0);
            target = sc.next().charAt(0);
            if (!nodes.containsKey(source)) nodes.put(source, new HashMap<>());
            if (!nodes.containsKey(target)) nodes.put(target, new HashMap<>());
            weight = Double.parseDouble(sc.next());
            nodes.get(source).put(target, weight);
        }
        sc.close();

        Visualizer vis = new Visualizer();
        vis.draw(nodes);

        Path shortest = find(first, last);
        return (shortest != null) ? shortest.getLiteral() : null;
    }

    protected abstract Path find(char first, char last);



    public static class Path {
        private String literal;
        private double length;

        public Path(String literal, double length) {
            this.literal = literal;
            this.length = length;
        }

        public Path(char literal, int length) {
            this.literal = "";
            this.literal += literal;
            this.length = length;
        }

        public Path addFront(char node, double length) {
            StringBuilder sb = new StringBuilder(this.literal);
            sb.insert(0, node);
            this.literal = sb.toString();
            this.length += length;
            return this;
        }

        public Path addBack(char node, double length) {
            this.literal += node;
            this.length += length;
            return this;
        }

        public String getLiteral() {
            return literal;
        }

        public double getLength() {
            return length;
        }

        public char getEnd() {
            return literal.charAt(literal.length() - 1);
        }
    }
}
