import java.util.Collections;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.Scanner;
import java.util.Map;

class SuperclassFirst {
    private static String solve() {
        Scanner sc = new Scanner(System.in);
        char first = sc.next().charAt(0);
        char last = sc.next().charAt(0);

        int len = (int) last - (int) first;
        HashMap<Character, HashMap<Character, Double>> nodes = new HashMap<>(len);
        for (char i = 'a'; i <= 'z'; i++) {
            nodes.put(i, new HashMap<>());
        }

        char source;
        char target;
        double weight;
        while (sc.hasNextLine()) {
            source = sc.next().charAt(0);
            target = sc.next().charAt(0);
            weight = Double.parseDouble(sc.next());
            nodes.get(source).put(target, weight);
        }
        sc.close();

        Path shortest = shortestPathFrom(nodes, first, last);
        return shortest.getLiteral();
    }

    private static Path shortestPathFrom(HashMap<Character, HashMap<Character, Double>> nodes, char start, char end) {
        if (start == end) return new Path(start, 0);
        if (nodes.get(start).isEmpty()) return null;

        double shortestLength = Collections.min(nodes.get(start).values());
        LinkedList<Path> paths = new LinkedList<>();
        for (Map.Entry<Character, Double> map : nodes.get(start).entrySet()) {
            if (map.getValue() == shortestLength) {
                Path path = shortestPathFrom(nodes, map.getKey(), end);
                if (path != null) {
                    paths.add(path.add(start, shortestLength));
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

    public static void main(String[] args) {
        System.out.println(solve());
    }

    private static class Path {
        private String literal;
        private double length;

        public Path(char literal, int length) {
            this.literal = "";
            this.literal += literal;
            this.length = length;
        }

        public Path add(char node, double length) {
            StringBuilder sb = new StringBuilder(this.literal);
            sb.insert(0, node);
            this.literal = sb.toString();
            this.length += length;
            return this;
        }

        public String getLiteral() {
            return literal;
        }

        public double getLength() {
            return length;
        }
    }
}
