package ds.node;

import java.util.function.Consumer;

import ds.misc.TripleConsumer;


public class NodeBuilder {
    private int physID;
    private int nodesNumber;
    private Boolean computeVirtual = null;

    private int[] connections;
    private int[] mapping;
    private int[] neighbors;
    private int[][] connectivity;

    TripleConsumer<String, Integer, Node> virtualCallback = (message, sender, self) -> {};
    Consumer<Node> initializationCallback = (node) -> {};

    public static NodeBuilder create(int physicalID, int[] neighbors) {
        return new NodeBuilder(physicalID, neighbors);
    }

    private NodeBuilder(int physicalID, int[] neighbors) {
        this.physID = physicalID;
        this.neighbors = neighbors;
        this.nodesNumber = neighbors.length;
    }

    public NodeBuilder defineVirtual(int[] mapping, int[] connections) {
        this.mapping = mapping;
        this.connections = connections;
        this.computeVirtual = false;
        if (connections.length != nodesNumber) throw new RuntimeException("Connection length (" + connections.length + ") doesn't match nodes number (" + nodesNumber + ")!");
        return this;
    }

    public NodeBuilder computeVirtual(int[][] connectivity) {
        this.computeVirtual = true;
        this.connectivity = connectivity;
        return this;
    }

    public NodeBuilder afterInitialization(Consumer<Node> initializationCallback) {
        this.initializationCallback = initializationCallback;
        return this;
    }

    public NodeBuilder onVirtualMessage(TripleConsumer<String, Integer, Node> virtualCallback) {
        this.virtualCallback = virtualCallback;
        return this;
    }

    public Node build() {
        if (computeVirtual == null) throw new RuntimeException("Virtual topology must be defined or computed!");
        Node node;
        if (computeVirtual) node = new Node(physID, nodesNumber, neighbors, null, connectivity, null, virtualCallback, initializationCallback);
        else node = new Node(physID, nodesNumber, neighbors, connections, null, mapping, virtualCallback, initializationCallback);
        return node;
    }
}
