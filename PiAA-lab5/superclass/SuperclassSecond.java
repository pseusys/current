import java.util.ArrayList;
import java.util.Scanner;

class SuperclassSecond {
    public static void main(String[] args) throws Exception {
        Scanner sc = new Scanner(System.in);
        String source = sc.nextLine();
        String proto = sc.nextLine();
        char wild = sc.nextLine().charAt(0);
        AhoCorasick ac = new AhoCorasick(5);
        ac.build(proto, wild);
        ac.solve(source);
    }

    public static class AhoCorasick {
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



    static class Node {
        public static char wild;
        enum NodeType {ROOT, INNER, INNER_LEAF, LEAF}

        Node.NodeType type;
        Character sign;

        Node parent;
        ArrayList<Node> children;
        Node def_link;

        public Node(char sign, Node parent, int alphbetLength) {
            this.sign = sign;
            this.parent = parent;
            if (parent.type == Node.NodeType.ROOT) def_link = parent;
            this.type = Node.NodeType.INNER;
            this.children = new ArrayList<>(alphbetLength);
        }

        public Node(int alphbetLength) {
            this.type = Node.NodeType.ROOT;
            this.children = new ArrayList<>(alphbetLength);
        }



        public void setLeaf() {
            if (this.type == Node.NodeType.INNER)
                if (this.children.isEmpty()) this.type = Node.NodeType.LEAF;
                else this.type = Node.NodeType.INNER_LEAF;
        }

        public void add(Node child) {
            this.children.add(child);
            if (this.type == Node.NodeType.LEAF) this.type = Node.NodeType.INNER_LEAF;
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
}
