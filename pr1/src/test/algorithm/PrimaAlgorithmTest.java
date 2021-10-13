package test.algorithm;

import classes.algorithm.PrimaAlgorithm;
import classes.graph.Ark;
import classes.graph.Graph;
import classes.graph.Node;
import classes.graph.NodePlus;
import org.junit.Before;
import org.junit.Test;

import java.awt.geom.Point2D;

import static org.junit.Assert.*;

public class PrimaAlgorithmTest {

    Graph graph;
    PrimaAlgorithm alg;

    @Before
    public void start(){
        graph = new Graph();
        graph.addNode(new NodePlus(new Point2D.Double(200, 400), "A"));
        graph.addNode(new NodePlus(new Point2D.Double(300, 600), "B"));
        graph.addNode(new NodePlus(new Point2D.Double(500, 600), "C"));

        graph.addArk("A", "B", 8);
        graph.addArk("B", "C", 9);

        alg = new PrimaAlgorithm();
    }

    @Test
    public void solve() {
        alg.solve(graph);
        assertEquals(alg.CountHidden(),graph.getNodes().size()-1);
    }

    @Test
    public void solveStep() {
        alg.prepareGraph(graph);
        alg.solveStep();
        assertNull(alg.solveStep());
    }


    @Test
    public void prepareGraph() {
        alg.prepareGraph(graph);
        int count_ark = 0;
        int count_node = 0;
        for (Ark ark: graph.getArks()){
            if (ark.isHidden()) {
                count_ark++;
            }
        }
        for (Node node: graph.getNodes()){
            if (node.isHidden()){
                count_node++;
            }
        }
        assertEquals(count_ark, 1);
        assertEquals(count_node, 1);
    }

}