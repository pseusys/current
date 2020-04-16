package code.wilderness;

public class AhoCorasick {
    int alphabetSize;
    String mark;
    Node root;

    public AhoCorasick(int size) {
        this.alphabetSize = size;
    }

    public void build(String mark, char wild) {
        Node.wild = wild;
        this.mark = mark;
        root = new Node(alphabetSize);
        Node current = root;
        for (char ch : mark.toCharArray()) {
            Node next = current.get(ch);
            if (next == null) {
                next = new Node(ch, current, alphabetSize);
                current.add(next);
            }
            current = next;
        }
        current.setLeaf();
    }

    public void solve(String src) {
        char [] source = src.toCharArray();
        Node current = root;
        for (int i = 0; i < source.length; i++) {
            Node next = current.get(source[i]);
            int counter = 0;
            while ((next == null) && (current != root)) {
                counter--;
                current = current.getDefLink();
                next = current.get(source[i]);
                printNode(current, i + counter);
            }
            if (next != null) current = next;
            printNode(current, i);
        }
    }

    private void printNode(Node node, int pos) {
        if ((node.type == Node.NodeType.LEAF) || (node.type == Node.NodeType.INNER_LEAF)) {
            System.out.println((pos+2 - mark.length()));
        }
    }
}
