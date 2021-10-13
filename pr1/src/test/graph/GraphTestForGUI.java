package test.graph;

import classes.Log;
import classes.graph.Graph;
import classes.graph.NodePlus;

import java.awt.geom.Point2D;

public class GraphTestForGUI {
    public static void runTest() {
        boolean successful = true;
        Log.gui(Log.Attributes.BOLD).col(Log.Colors.GREEN).say("Тест создания графа начался!");
        Graph graph = new Graph();

        Log.gui().say("Проверяем, пустой ли граф:");
        successful &= graph.isEmpty();
        Log.gui().say(graph.isEmpty());
        if (!graph.isEmpty())
            Log.gui(Log.Attributes.ITALIC, Log.Attributes.BOLD).col(Log.Colors.RED).say("!Граф не пустой!");

        Log.gui().say("Создаём узел с именем 'Node':");
        graph.addNode(new NodePlus(new Point2D.Double(0, 0), "Node"));
        successful &= (graph.getNodes().size() == 1);
        if (!(graph.getNodes().size() == 1))
            Log.gui(Log.Attributes.ITALIC, Log.Attributes.BOLD).col(Log.Colors.RED).say("!Узел не создан!");

        Log.gui().say("Создаём узел с именем 'Node1':");
        graph.addNode(new NodePlus(new Point2D.Double(15, 15), "Node1"));
        successful &= (graph.getNodes().size() == 2);
        if (!(graph.getNodes().size() == 2))
            Log.gui(Log.Attributes.ITALIC, Log.Attributes.BOLD).col(Log.Colors.RED).say("!Узел не создан!");


        Log.gui().say("Создаём ребро с весом 7:");
        graph.addArk("Node","Node1", 7);
        successful &= (graph.getArks().size() == 1);
        if (!(graph.getArks().size() == 1))
            Log.gui(Log.Attributes.ITALIC, Log.Attributes.BOLD).col(Log.Colors.RED).say("!Ребро не создано!");


        Log.gui().say("Удаляем ребро с весом 7:");
        graph.deleteArk("Node","Node1");
        successful &= (graph.getArks().size() == 0);
        if (!(graph.getArks().size() == 0))
            Log.gui(Log.Attributes.ITALIC, Log.Attributes.BOLD).col(Log.Colors.RED).say("!Ребро не удалено!");

        Log.gui().say("Удаляем узел с именем 'Node1':");
        graph.deleteNode("Node1");
        successful &= (graph.getNodes().size() == 1);
        if (!(graph.getNodes().size() == 1))
            Log.gui(Log.Attributes.ITALIC, Log.Attributes.BOLD).col(Log.Colors.RED).say("!Узел не удален!");

        Log.gui().say("Удаляем узел с именем 'Node':");
        graph.deleteNode("Node");
        successful &= (graph.getNodes().size() == 0);
        if (!(graph.getNodes().size() == 0))
            Log.gui(Log.Attributes.ITALIC, Log.Attributes.BOLD).col(Log.Colors.RED).say("!Узел не удален!");

        Log.gui(Log.Attributes.BOLD).end("!").col(successful ? Log.Colors.GREEN : Log.Colors.RED).say("Тестирование прошло ", successful ? "успешно" : "провально");
    }
}
