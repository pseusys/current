package code.aho_corasick;

import java.util.List;

public class AhoCorasick {
    int alphabetSize;
    List<String> marks;
    Node root;

    public AhoCorasick(int size) {
        this.alphabetSize = size;
    }

    public void build(List<String> marks) {
        this.marks = marks;
        root = new Node(alphabetSize);
        for (int i = 0; i < marks.size(); i++) {
            Node current = root;
            for (char ch : marks.get(i).toCharArray()) {
                Node next = current.get(ch);
                if (next == null) {
                    next = new Node(ch, current, alphabetSize);
                    current.add(next);
                }
                current = next;
            }
            current.setLeaf(i);
        }
    }

    public void solve(String src, boolean searchForInner) {
        char [] source = src.toCharArray();
        Node current = root;
        for (int i = 0; i < source.length; i++) {
            Node next = current.get(source[i]);
            int counter = 0;
            while ((next == null) && (current != root)) {
                counter--;
                current = current.getDefLink();
                next = current.get(source[i]);
                if (printNode(current, i + counter))
                    if (!searchForInner) current = root;
            }
            if (next != null) current = next;
            if (printNode(current, i))
                if (!searchForInner) current = root;
        }
    }

    private boolean printNode(Node node, int pos) {
        if ((node.type == Node.NodeType.LEAF) || (node.type == Node.NodeType.INNER_LEAF)) {
            System.out.println((pos+2 - marks.get(node.leafNumber).length()) + " " + (node.leafNumber+1));
            return true;
        }
        return false;
    }
}
