package ds.node;

import java.io.IOException;
import java.util.Arrays;
import java.util.function.BiFunction;
import java.util.function.Function;

import com.rabbitmq.client.Channel;
import com.rabbitmq.client.DeliverCallback;
import com.rabbitmq.client.Delivery;

import ds.base.BaseMessage;
import ds.misc.PermutationCalculator;
import ds.objects.ConnectionMessage;
import ds.objects.DataMessage;
import ds.objects.RoutingMessage;
import ds.objects.ServiceMessage;
import ds.objects.ServiceMessage.MessageType;


public class NodeMessageCallback implements DeliverCallback {
    private final Node node;
    private int round_counter = 0;
    private boolean compute;

    private final int[] neighbors;
    private final Boolean[] round_received;
    private final Boolean[] initialized;

    private PermutationCalculator calculator;
    private final Channel channel;
    private final int[][] connectivity;
    private final int[][] distances;
    private final int[][] mappings;

    protected NodeMessageCallback(Node node, int[] neighbors, Channel channel, int[][] connectivity) {
        this.node = node;
        this.neighbors = neighbors;
        this.channel = channel;
        this.compute = connectivity != null;
        this.connectivity = connectivity;

        this.round_received = new Boolean[node.nodesCount()];
        init(round_received, (idx) -> neighbors[idx] == -1);
        this.initialized = new Boolean[node.nodesCount()];
        init(initialized, (idx) -> false);
        this.distances = new int[node.nodesCount()][];
        init(distances, (idx) -> null);
        this.mappings = new int[node.nodesCount()][];
        init(mappings, (idx) -> null);
    }

    @Override
    public synchronized void handle(String consumerTag, Delivery delivery) throws IOException {
        BaseMessage message;
        try {
            message = BaseMessage.parse(delivery.getBody());
        } catch (ClassNotFoundException e) {
            throw new RuntimeException(node.physicalRepresentation() + " couldn't parse a message it received!");
        }
        boolean ack = true;

        if (message.getClass().getName() == ServiceMessage.class.getName()) {
            ServiceMessage sm = (ServiceMessage) message;
            switch (sm.type) {
                case INITIALIZED:
                    if (initialized[sm.sender]) break;
                    node.broadcastMessagePhysical(sm);
                    initialized[sm.sender] = true;
                    if (check(initialized, (idx, val) -> val)) node.initializationCallback.accept(node);
                    break;
                    
                case CASCADE_DEATH:
                    node.die(sm.sender);
                    break;

                default:
                    System.out.println("Unexpected service message received (" + sm.type.name() + ")!");
                    break;
            }

        } else if (message.getClass().getName() == DataMessage.class.getName()) {
            DataMessage dm = (DataMessage) message;
            if (dm.receiver == node.getVirtualID()) {
                if (node.virtualCallback != null) node.virtualCallback.apply(dm.message, dm.sender, node);
                else System.out.println(node.virtualRepresentation() + " received a message, but it doesn't have a callback for it!");
            } else {
                System.out.println(node.virtualRepresentation() + " forwards a message (from " + dm.sender + ", to: " + dm.receiver + ")!");
                node.forwardMessageVirtual(message, dm.receiver);
            }

        } else if (message.getClass().getName() == ConnectionMessage.class.getName()) {
            ConnectionMessage cm = (ConnectionMessage) message;
            // Computed mapping scores.
            if (cm.computed && mappings[cm.sender] == null) {
                node.broadcastMessagePhysical(cm);
                mappings[cm.sender] = cm.permutation;
                if (check(mappings, (idx, val) -> val != null)) chooseMapping();
            }
            if (!cm.computed && distances[cm.sender] == null) {
                node.broadcastMessagePhysical(cm);
                distances[cm.sender] = cm.permutation;
                if (check(distances, (idx, val) -> val != null)) computeMapping();
            }

        } else if (message.getClass().getName() == RoutingMessage.class.getName()) {
            RoutingMessage rm = (RoutingMessage) message;
            ack = !round_received[rm.sender];
            node.getRoutingTable().update(rm.table, rm.sender);
            round_received[rm.sender] = true;
            if (check(round_received, (idx, val) -> val)) nextRound();

        } else System.out.println("Unexpected message received (" + message.getClass().getSimpleName() + ")!");

        if (channel.isOpen()) {
            if (ack) channel.basicAck(delivery.getEnvelope().getDeliveryTag(), false);
            else channel.basicNack(delivery.getEnvelope().getDeliveryTag(), false, true);
        }
    }

    protected void initialize() {
        node.broadcastMessagePhysical(new RoutingMessage(node.getRoutingTable(), node.getPhysicalID()));
    }

    private <T> boolean check(T[] arr, BiFunction<Integer, T, Boolean> comparator) {
        boolean init = true;
        for (int i = 0; i < arr.length; i++) init &= comparator.apply(i, arr[i]);
        return init;
    }

    private <T> void init(T[] arr, Function<Integer, T> initializer) {
        for (int i = 0; i < node.nodesCount(); i++) arr[i] = initializer.apply(i);
    }

    // Possible improvement: reduce number of rounds? Check from what nodes info already received?
    private void nextRound() {
        init(round_received, (idx) -> neighbors[idx] == -1);
        if (round_counter == node.nodesCount()) {
            System.out.println(node.physicalRepresentation() + " has routes: " + Arrays.toString(node.physicalDistances()));
            initialized[node.getPhysicalID()] = true;
            if (compute) {
                int[] dists = node.physicalDistances();
                distances[node.getPhysicalID()] = dists;
                node.broadcastMessagePhysical(new ConnectionMessage(dists, node.getPhysicalID(), false));
                if (check(distances, (idx, val) -> val != null)) computeMapping();
            } else {
                node.broadcastMessagePhysical(new ServiceMessage(node.getPhysicalID(), MessageType.INITIALIZED));
                if (check(initialized, (idx, val) -> val)) node.initializationCallback.accept(node);
            }
        } else node.broadcastMessagePhysical(new RoutingMessage(node.getRoutingTable(), node.getPhysicalID()));
        round_counter++;
    }

    private void computeMapping() {
        calculator = new PermutationCalculator(distances, connectivity);
        int[] perm = calculator.calculateBestPermutationForShare(node.getPhysicalID());
        mappings[node.getPhysicalID()] = perm;
        node.broadcastMessagePhysical(new ConnectionMessage(perm, node.getPhysicalID(), true));
        if (check(mappings, (idx, val) -> val != null)) chooseMapping();
    }

    private void chooseMapping() {
        int[] minPerm = mappings[node.getPhysicalID()];
        int minScore = calculator.calculatePermutationScore(minPerm);
        for (int i = 0; i < mappings.length; i++) {
            int curScore = calculator.calculatePermutationScore(mappings[i]);
            if (curScore <= minScore) {
                minScore = curScore;
                minPerm = mappings[i];
            }
        }
        System.out.println(node.physicalRepresentation() + " has chosen mapping: " + Arrays.toString(minPerm));
        node.setVirtualMappings(minPerm, connectivity);
        node.initializationCallback.accept(node);
    }
}
