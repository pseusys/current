package classes.graphic;

class Ark {
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
