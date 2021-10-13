package test.algorithm;

import classes.Filer;
import classes.Log;
import classes.Settings;
import classes.algorithm.PrimaAlgorithm;
import classes.graph.Graph;
import classes.shapes.GraphShape;

import javax.swing.*;
import java.awt.*;

public class PrimaAlgorithmTestForGUI {
    public static void runTest(Graph graph) {
        boolean success = true;

        PrimaAlgorithm alg = new PrimaAlgorithm();
        alg.solve(graph);

        Log.gui().say("Проверяем правильность решения");
        success &= (alg.CountHidden() == (graph.getNodes().size()-1));
        if (!(alg.CountHidden() == (graph.getNodes().size()-1)))
            Log.gui(Log.Attributes.ITALIC,Log.Attributes.BOLD ).col(Log.Colors.RED).say("!Остовное дерево не минимально!");


        Log.gui(Log.Attributes.BOLD).end("!").col(success ? Log.Colors.GREEN : Log.Colors.RED).say("Тестирование прошло ", success ? "успешно" : "провально! ");
    }
}
