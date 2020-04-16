package classes.graphic;

import java.util.*;

public class Net {
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
        boolean passed = true;
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
