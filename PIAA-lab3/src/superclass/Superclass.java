package superclass;

import java.io.PrintStream;
import java.util.*;

class Superclass {
    public static void main(String[] args) {
        Scanner sc = new Scanner(System.in);
        Pathfinder pf = new Pathfinder(sc);
        pf.solve(System.out);
        sc.close();
    }

    static class Pathfinder {
        private Net net;

        public Pathfinder(Scanner sc) {
            int arks = Integer.parseInt(sc.next());

            char first = sc.next().charAt(0);
            char last = sc.next().charAt(0);

            this.net = new Net(first, last);

            char source;
            char target;
            int capacity;
            while (sc.hasNextLine()) {
                source = sc.next().charAt(0);
                target = sc.next().charAt(0);
                capacity = Integer.parseInt(sc.next());
                net.putArk(source, target, capacity);
            }
        }

        public void solve(PrintStream ps) {
            Path path = new Path(net, net.getSource());
            int lastVisitedPos = -1;
            while (!path.isDeadEnd()) {
                if (path.getEnd().getNode() == net.getExit()) {
                    operate(path);
                }
                ArrayList<Path.DirectedNode> neighbours = path.getTopNeighbours();
                if ((lastVisitedPos + 1 == neighbours.size()) || (path.getEnd().getNode() == net.getExit())) {
                    Path.DirectedNode lastVisited = path.popNode();
                    if (path.isDeadEnd()) break;
                    lastVisitedPos = path.findLastInNeighbours(lastVisited);
                } else {
                    Path.DirectedNode next = neighbours.get(++lastVisitedPos);
                    if (path.pushNode(next)) lastVisitedPos = -1;
                }
            }

            net.optimizeArks();

            //if (net.check()) {
            ps.println(net.getResultThrough());
            Map<Character, Map<Character, Integer>> oldGraph = net.getOldStyleGraph();
            for (Map.Entry<Character, Map<Character, Integer>> node: oldGraph.entrySet()) {
                for (Map.Entry<Character, Integer> ark : node.getValue().entrySet()) {
                    ps.println(node.getKey() + " " + ark.getKey() + " " + ark.getValue());
                }
            }
            //} else ps.println("Operation failure, aborting.");
        }

        private void operate(Path path) {
            int minCapacity = path.getMinCapacity();
            path.modifyFilled(() -> minCapacity);
        }
    }

    static class Net {
        private Node source, exit;
        private HashMap<Character, Node> table;

        public Net(char source, char exit) {
            this.table = new HashMap<>();
            this.source = new Node(source);
            this.exit = new Node(exit);

            table.put(source, this.source);
            table.put(exit, this.exit);
        }

        public void putArk(char from, char to, int capacity) {
            Node fromN, toN;

            if (!table.containsKey(from)) {
                fromN = new Node(from);
                table.put(from, fromN);
            } else fromN = table.get(from);

            if (!table.containsKey(to)) {
                toN = new Node(to);
                table.put(to, toN);
            } else toN = table.get(to);

            Ark between = new Ark(fromN, toN, capacity);
            fromN.putArk(between);
            toN.putArk(between);
        }

        public char getSource() {
            return source.getName();
        }

        public char getExit() {
            return exit.getName();
        }

        public boolean check() {
            boolean passed = (source.inArksNum() == 0) && (exit.outArksNum() == 0);
            for (Map.Entry<Character, Node> node : table.entrySet()) {
                if ((node.getValue() == source) || (node.getValue() == exit)) continue;
                passed &= node.getValue().inEqualsOut();
            }
            return passed;
        }

        public int getResultThrough() {
            return source.getOut();
        }

        public Map<Character, Map<Character, Integer>> getOldStyleGraph() {
            Map<Character, Map<Character, Integer>> old = new TreeMap<>();
            for (Map.Entry<Character, Node> node : table.entrySet()) {
                Map<Character, Integer> ark = new TreeMap<>();
                for (char leaf : node.getValue().getAncestors()) {
                    ark.put(leaf, getArk(node.getKey(), leaf).getFilled());
                }
                old.put(node.getKey(), ark);
            }
            return old;
        }

        public void optimizeArks() {
            for (Map.Entry<Character, Node> node : table.entrySet()) {
                ArrayList<Character> anc = node.getValue().getAncestors();
                ArrayList<Character> pred = node.getValue().getPredecessors();
                for (Character a : anc) {
                    if (pred.contains(a)) {
                        Ark in = getArk(node.getKey(), a);
                        Ark out = getArk(a, node.getKey());
                        int delta = Math.min(in.getFilled(), out.getFilled());
                        in.modifyFilled(-delta);
                        out.modifyFilled(-delta);
                    }
                }
            }
        }



        Node getNode(char node) {
            return table.get(node);
        }

        Ark getArk(char from, char to) {
            if (!table.containsKey(from) || !table.containsKey(to)) return null;
            return table.get(from).arkTo(to);
        }

        Ark getArkDirect(char from, char to) {
            Ark forth = getArk(from, to);
            Ark back = getArk(to, from);
            return forth == null ? back : forth;
        }
    }

    static class Node {
        private char name;
        private int in, out;
        private HashMap<Character, Ark> inArks;
        private HashMap<Character, Ark> outArks;

        Node(char name) {
            this.name = name;
            this.in = this.out = 0;
            this.inArks = new HashMap<>();
            this.outArks = new HashMap<>();
        }

        char getName() {
            return name;
        }

        int getIn() {
            return in;
        }

        int getOut() {
            return out;
        }

        ArrayList<Character> getNeighbours() {
            ArrayList<Character> neighbours = new ArrayList<>(outArks.keySet());
            neighbours.addAll(inArks.keySet());
            neighbours.sort(Comparator.naturalOrder());
            return neighbours;
        }

        ArrayList<Character> getAncestors() {
            ArrayList<Character> ancestors = new ArrayList<>(outArks.keySet());
            ancestors.sort(Comparator.naturalOrder());
            return ancestors;
        }

        ArrayList<Character> getPredecessors() {
            ArrayList<Character> predecessors = new ArrayList<>(inArks.keySet());
            predecessors.sort(Comparator.naturalOrder());
            return predecessors;
        }

        void putArk(Ark nova) {
            if (nova.getFrom().getName() == name) outArks.put(nova.getTo().getName(), nova);
            else if (nova.getTo().getName() == name) inArks.put(nova.getFrom().getName(), nova);
        }

        Ark arkTo(char to) {
            return outArks.get(to);
        }

        void modifyIn(int delta) {
            this.in += delta;
        }

        void modifyOut(int delta) {
            this.out += delta;
        }

        int inArksNum() {
            return inArks.size();
        }

        int outArksNum() {
            return outArks.size();
        }

        boolean inEqualsOut() {
            return in == out;
        }
    }

    static class Ark {
        private Node from, to;
        private int capacity, filled;

        Ark(Node from, Node to, int capacity) {
            this.from = from;
            this.to = to;
            this.capacity = capacity;
            this.filled = 0;
        }

        Node getFrom() {
            return from;
        }

        Node getTo() {
            return to;
        }

        void modifyFilled(int delta) {
            this.filled += delta;
        }

        int getCapacity() {
            return capacity - filled;
        }

        int getFilled() {
            return filled;
        }
    }

    static class Path {
        private Net base;
        private LinkedList<DirectedNode> roadMap;

        public Path(Net base, char beginning) {
            this.base = base;
            this.roadMap = new LinkedList<>();
            pushNode(new DirectedNode(beginning, false));
        }

        public boolean pushNode(DirectedNode node) {
            for (DirectedNode nd : roadMap) {
                if (nd.getNode() == node.getNode()) return false;
            }
            if ((node.getNode() == base.getSource()) && (node.isDirection())) return false;
            if ((node.getNode() == base.getExit()) && (!node.isDirection())) return false;
            roadMap.push(node);
            return true;
        }

        public DirectedNode popNode() {
            return roadMap.pop();
        }

        public DirectedNode getEnd() {
            return roadMap.isEmpty() ? null : roadMap.peek();
        }

        public boolean isDeadEnd() {
            return roadMap.isEmpty();
        }

        public ArrayList<DirectedNode> getTopNeighbours() {
            if (roadMap.isEmpty()) return null;
            ArrayList<DirectedNode> result = new ArrayList<>();
            ArrayList<Character> ancestors = base.getNode(roadMap.peek().getNode()).getAncestors();
            ArrayList<Character> predecessors = base.getNode(roadMap.peek().getNode()).getPredecessors();
            for (Character anc : ancestors) {
                result.add(new DirectedNode(anc, true));
            }
            for (Character pred : predecessors) {
                result.add(new DirectedNode(pred, false));
            }
            return result;
        }

        public int findLastInNeighbours(DirectedNode node) {
            ArrayList<DirectedNode> neighbours = getTopNeighbours();
            for (int i = 0; i < neighbours.size(); i++) {
                if (neighbours.get(i).equals(node)) return i;
            }
            return -1;
        }



        public int getMinCapacity() {
            if (roadMap.size() < 2) return -1;
            int minCapacity;
            if (roadMap.get(roadMap.size()-2).isDirection())
                minCapacity = base.getArk(roadMap.get(roadMap.size() - 1).getNode(), roadMap.get(roadMap.size() - 2).getNode()).getCapacity();
            else
                minCapacity = base.getArk(roadMap.get(roadMap.size() - 2).getNode(), roadMap.get(roadMap.size() - 1).getNode()).getFilled();
            int capacity;
            Ark ark;
            for (int i = roadMap.size() - 3; i >= 0; i--) {
                if (roadMap.get(i).isDirection()) {
                    ark = base.getArk(roadMap.get(i+1).getNode(), roadMap.get(i).getNode());
                    capacity = ark.getCapacity();
                } else {
                    ark = base.getArk(roadMap.get(i).getNode(), roadMap.get(i+1).getNode());
                    capacity = ark.getFilled();
                }
                if (capacity < minCapacity) {
                    minCapacity = capacity;
                }
            }
            return minCapacity;
        }

        public void modifyFilled(Arked runner) {
            if (roadMap.size() < 2) return;
            Ark ark;
            for (int i = roadMap.size() - 2; i >= 0; i--) {
                if (roadMap.get(i).isDirection()) {
                    ark = base.getArk(roadMap.get(i+1).getNode(), roadMap.get(i).getNode());
                    ark.modifyFilled(runner.modify());
                    ark.getFrom().modifyOut(runner.modify());
                    ark.getTo().modifyIn(runner.modify());
                } else {
                    ark = base.getArk(roadMap.get(i).getNode(), roadMap.get(i+1).getNode());
                    ark.modifyFilled(-runner.modify());
                    ark.getFrom().modifyOut(-runner.modify());
                    ark.getTo().modifyIn(-runner.modify());
                }
            }
        }

        public interface Arked {
            int modify();
        }



        @Override
        public String toString() {
            StringBuilder b = new StringBuilder();
            for (int i = roadMap.size() - 1; i >= 0; i--) {
                b.append(roadMap.get(i));
                b.append(' ');
            }
            return "Path: " + b.toString();
        }



        public static class DirectedNode {
            private char node;
            private boolean direction;

            public DirectedNode(char node, boolean direction) {
                this.node = node;
                this.direction = direction;
            }

            public char getNode() {
                return node;
            }

            public boolean isDirection() {
                return direction;
            }

            public boolean equals(DirectedNode other) {
                return (this.node == other.node) && (this.direction == other.direction);
            }

            @Override
            public String toString() {
                return String.valueOf(node) + (direction ? '+' : '-');
            }
        }
    }
}
