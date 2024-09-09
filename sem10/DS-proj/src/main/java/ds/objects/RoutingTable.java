package ds.objects;

import java.io.Serializable;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;


public class RoutingTable implements Serializable {
    public class RoutingTableEntry implements Serializable {
        public final int gate, destination, distance;

        protected RoutingTableEntry(int gate, int destination, int distance) {
            this.gate = gate;
            this.destination = destination;
            this.distance = distance;
        }

        @Override
        public String toString() {
            return "(" + gate + ", " + destination + ", " + distance + ")";
        }
    }

    private final List<RoutingTableEntry> table = Collections.synchronizedList(new ArrayList<RoutingTableEntry>());

    public RoutingTable(int current, int neighbors) {
        for (int i = 0; i < neighbors; i++) {
            int gate, destination, distance;
            if (i == current) {
                gate = destination = current;
                distance = 0;
            } else {
                gate = -1;
                destination = i;
                distance = Integer.MAX_VALUE;
            }
            table.add(new RoutingTableEntry(gate, destination, distance));
        }
    }

    public RoutingTableEntry path(int node) {
        for (RoutingTableEntry entry: table) if (entry.destination == node) return entry;
        throw new RuntimeException("Route to node " + node + " is not defined!");
    }

    public synchronized void update(RoutingTable another, int sender) {
        for (RoutingTableEntry entry: another.table) {
            int pathIndex = -1;
            for (int i = 0; i < table.size(); i++) if (table.get(i).destination == entry.destination) {
                pathIndex = i;
                break;
            }
            if (pathIndex == -1) throw new RuntimeException("Route to node " + entry + " is not defined!");
            RoutingTableEntry path = table.get(pathIndex);
            if (entry.distance < path.distance) table.set(pathIndex, new RoutingTableEntry(sender, path.destination, entry.distance + 1));
        }
    }

    @Override
    public String toString() {
        return table.toString();
    }
}
