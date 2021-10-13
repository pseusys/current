package test.graph;

import classes.Filer;
import classes.Log;
import classes.graph.Graph;
import classes.graph.NodePlus;
import org.junit.Before;
import org.junit.Test;

import java.awt.geom.Point2D;

import static org.junit.Assert.*;

public class GraphTest {

    Graph graph;

    @Before
    public void start(){
        //Log.gui(Log.Attributes.BOLD).attr(Log.Attributes.BOLD).col(Log.Colors.GREEN).say("Тест начался!");
        graph = new Graph();
        graph.addNode(new NodePlus(new Point2D.Double(200, 400), "A"));
        graph.addNode(new NodePlus(new Point2D.Double(300, 600), "B"));
        graph.addNode(new NodePlus(new Point2D.Double(500, 600), "C"));

        graph.addArk("A", "B", 8);
        graph.addArk("B", "C", 9);

    }

    @Test
    public void addNode() {
        graph.addNode(new NodePlus(new Point2D.Double(0, 0), "Node"));
        assertEquals(graph.getNodes().size(), 4);
    }

    @Test
    public void deleteNode() {

        graph.deleteNode("A");
        assertEquals(graph.getNodes().size(), 2);
    }

    @Test
    public void changeNode() {
        graph.changeNode("A", "Q");

    }

    @Test
    public void addArk() {
        graph.addArk("A","C", 7);
        assertEquals(graph.getArks().size(), 3);
    }


    @Test
    public void deleteArk() {
        graph.deleteArk("A","B");
        assertEquals(graph.getArks().size(), 1);
    }


    @Test
    public void isEmpty() {
        assertFalse(graph.isEmpty());
    }


}