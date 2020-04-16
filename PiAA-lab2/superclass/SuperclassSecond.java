import java.util.*;

class SuperclassSecond {
    static HashMap<Character, HashMap<Character, Double>> nodes = new HashMap<>();

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

    public static String solve() {
        Scanner sc = new Scanner(System.in);
        char first = sc.next().charAt(0);
        char last = sc.next().charAt(0);

        int len = (int) last - (int) first;
        nodes = new HashMap<>(len);
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

        Path shortest = find(first, last);
        return (shortest != null) ? shortest.getLiteral() : null;
    }

    private static char Ofirst, Olast;

    private static double g(Path path) {
        return path.getLength();
    }

    private static double h(Path path) {
        return Math.abs((int) Olast - (int) path.getEnd());
    }

    private static double f(Path path) {
        return g(path) + h(path);
    }

    public static Path find(char first, char last) {
        Ofirst = first;
        Olast = last;

        LinkedList<Path> closed = new LinkedList<>();
        TreeMap<Double, Path> opened = new TreeMap<>();

        Path beginning = new Path(first, 0);
        opened.put(f(beginning), beginning);

        while (!opened.isEmpty()) {
            Map.Entry<Double, Path> current = opened.firstEntry();

            if (current.getValue().getEnd() == last) return current.getValue();

            opened.remove(current.getKey());
            closed.push(current.getValue());

            for (Map.Entry<Character, Double> near: nodes.get(current.getValue().getEnd()).entrySet()) {
                Path vertex = new Path(current.getValue().getLiteral(), current.getValue().getLength())
                        .addBack(near.getKey(), near.getValue());

                if (!contains(closed, vertex))
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

        return null;
    }

    private static boolean contains(LinkedList<Path> paths, Path path) {
        for (Path p : paths) {
            if (p.getEnd() == path.getEnd()) return true;
        }
        return false;
    }

    private static boolean contains(TreeMap<Double, Path> paths, Path path) {
        for (Path p : paths.values()) {
            if (p.getEnd() == path.getEnd()) return true;
        }
        return false;
    }

    private static double getDistance(TreeMap<Double, Path> paths, Path path) {
        for (Map.Entry<Double, Path> p : paths.entrySet()) {
            if (p.getValue().getEnd() == path.getEnd()) return p.getKey();
        }
        return -1;
    }

    public static void main(String[] args) {
        System.out.println(solve());
    }
}
