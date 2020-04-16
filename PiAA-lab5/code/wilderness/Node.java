package code.wilderness;

import java.util.ArrayList;

class Node {
    public static char wild;
    enum NodeType {ROOT, INNER, INNER_LEAF, LEAF}

    NodeType type;
    Character sign;

    Node parent;
    ArrayList<Node> children;
    Node def_link;

    public Node(char sign, Node parent, int alphbetLength) {
        this.sign = sign;
        this.parent = parent;
        if (parent.type == NodeType.ROOT) def_link = parent;
        this.type = NodeType.INNER;
        this.children = new ArrayList<>(alphbetLength);
    }

    public Node(int alphbetLength) {
        this.type = NodeType.ROOT;
        this.children = new ArrayList<>(alphbetLength);
    }



    public void setLeaf() {
        if (this.type == NodeType.INNER)
            if (this.children.isEmpty()) this.type = NodeType.LEAF;
            else this.type = NodeType.INNER_LEAF;
    }

    public void add(Node child) {
        this.children.add(child);
        if (this.type == NodeType.LEAF) this.type = NodeType.INNER_LEAF;
    }

    public Node getDefLink() {
        if (def_link == null) {
            def_link = parent.getDefLink().pass(sign);
        }
        return def_link;
    }

    public Node get(char letter) {
        for (Node child : children) {
            if ((child.sign == letter) || (child.sign == wild)) return child;
        }
        return null;
    }

    private Node pass(char letter) {
        for (Node child : children) {
            if ((child.sign == letter) || (child.sign == wild)) return child;
        }
        if (type == Node.NodeType.ROOT) return this;
        else return getDefLink().pass(letter);
    }
}
