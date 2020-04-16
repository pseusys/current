import java.util.ArrayList;
import java.util.List;
import java.util.Scanner;

class SuperclassFirst {
    public static void main(String[] args) throws Exception {
        Scanner sc = new Scanner(System.in);
        String source = sc.nextLine();
        int number = Integer.parseInt(sc.nextLine());
        ArrayList<String> marks = new ArrayList<>();
        for (int i = 0; i < number; i++) {
            marks.add(sc.nextLine());
        }
        AhoCorasick ac = new AhoCorasick(5);
        ac.build(marks);
        ac.solve(source, true);
    }

    public static class AhoCorasick {
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



    static class Node {
        enum NodeType {ROOT, INNER, INNER_LEAF, LEAF}

        Node.NodeType type;
        Integer leafNumber;
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



        public void setLeaf(int leafNumber) {
            this.leafNumber = leafNumber;
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
                if (child.sign == letter) return child;
            }
            return null;
        }

        private Node pass(char letter) {
            for (Node child : children) {
                if (child.sign == letter) return child;
            }
            if (type == Node.NodeType.ROOT) return this;
            else return getDefLink().pass(letter);
        }
    }
}
