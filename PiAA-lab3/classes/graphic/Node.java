package classes.graphic;

import java.util.ArrayList;
import java.util.Comparator;
import java.util.HashMap;

class Node {
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
