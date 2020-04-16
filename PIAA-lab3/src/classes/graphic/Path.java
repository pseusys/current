package classes.graphic;

import java.io.PrintStream;
import java.util.ArrayList;
import java.util.LinkedList;

public class Path {
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
