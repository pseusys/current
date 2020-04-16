package classes;

import java.util.HashMap;
import java.util.LinkedList;

public class Graph {
    HashMap<Character, Node> table;

    public void putArk(char from, char to, int capacity) {
        Node fromN, toN;

        if (!table.containsKey(from)) {
            fromN = new Node(from);
        } else fromN = table.get(from);

        if (!table.containsKey(to)) {
            toN = new Node(to);
        } else toN = table.get(to);

        Ark between = new Ark(fromN, toN, capacity);
        
    }



    private class Ark {
        Node from, to;
        int capacity, filled;

        public Ark(Node from, Node to, int capacity) {
            this.from = from;
            this.to = to;
            this.capacity = capacity;
            this.filled = 0;
        }
    }

    private class Node {
        char name;
        int in, out;
        LinkedList<Ark> arks;

        public Node(char name) {
            this.name = name;
            this.in = this.out = 0;
            this.arks = new LinkedList<>();
        }
    }
}
