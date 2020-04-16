package classes;

import classes.graphic.Net;
import classes.graphic.Path;

import java.io.PrintStream;
import java.util.ArrayList;
import java.util.Map;
import java.util.Scanner;

public class Pathfinder {
    private Net net;

    public Pathfinder(Scanner sc) {
        int arks = Integer.parseInt(sc.next());

        char first = sc.next().charAt(0);
        char last = sc.next().charAt(0);

        this.net = new Net(first, last);

        char source;
        char target;
        int capacity;
        while (sc.hasNextLine()) {
            source = sc.next().charAt(0);
            target = sc.next().charAt(0);
            capacity = Integer.parseInt(sc.next());
            net.putArk(source, target, capacity);
        }
    }

    public void solve(PrintStream ps) {
        Path path = new Path(net, net.getSource());
        int lastVisitedPos = -1;
        while (!path.isDeadEnd()) {
            if (path.getEnd().getNode() == net.getExit()) {
                operate(path);
            }
            ArrayList<Path.DirectedNode> neighbours = path.getTopNeighbours();
            if ((lastVisitedPos + 1 == neighbours.size()) || (path.getEnd().getNode() == net.getExit())) {
                Path.DirectedNode lastVisited = path.popNode();
                if (path.isDeadEnd()) break;
                lastVisitedPos = path.findLastInNeighbours(lastVisited);
            } else {
                Path.DirectedNode next = neighbours.get(++lastVisitedPos);
                if (path.pushNode(next)) lastVisitedPos = -1;
            }
        }

        net.optimizeArks();

        if (net.check()) {
            ps.println(net.getResultThrough());
            Map<Character, Map<Character, Integer>> oldGraph = net.getOldStyleGraph();
            for (Map.Entry<Character, Map<Character, Integer>> node: oldGraph.entrySet()) {
                for (Map.Entry<Character, Integer> ark : node.getValue().entrySet()) {
                    ps.println(node.getKey() + " " + ark.getKey() + " " + ark.getValue());
                }
            }
        } else ps.println("Operation failure, aborting.");
    }

    private void operate(Path path) {
        int minCapacity = path.getMinCapacity();
        path.modifyFilled(() -> minCapacity);
    }
}
