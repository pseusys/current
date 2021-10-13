package test;

import classes.Filer;
import classes.Log;
import classes.graph.Graph;
import test.algorithm.PrimaAlgorithmTestForGUI;
import test.graph.GraphTestForGUI;

public class PrimaTest {
    public static void runTests() {
        GraphTestForGUI.runTest();

        Filer.loadGraphFromResources(Filer.SAMPLE_GRAPH, new Filer.OnGraphLoaded() {
            @Override
            public void onFinished(Graph graph, Exception reason) {
                if (reason != null){
                    Log.gui(Log.Attributes.BOLD).col(Log.Colors.RED).say("В файле нет графа!");
                }else{
                    Log.gui(Log.Attributes.BOLD).col(Log.Colors.GREEN).say("Тест алгоритма для графа номер 1 начался!");
                    PrimaAlgorithmTestForGUI.runTest(graph);
                }
            }
        });

        Filer.loadGraphFromResources(Filer.PETERSON_GRAPH, new Filer.OnGraphLoaded() {
            @Override
            public void onFinished(Graph graph, Exception reason) {
                if (reason != null){
                    Log.gui(Log.Attributes.BOLD).col(Log.Colors.RED).say("В файле нет графа!");
                }else{
                    Log.gui(Log.Attributes.BOLD).col(Log.Colors.GREEN).say("Тест алгоритма для графа номер 2 начался!");
                    PrimaAlgorithmTestForGUI.runTest(graph);
                }
            }
        });

        Filer.loadGraphFromResources(Filer.BIPARTITE_GRAPH, new Filer.OnGraphLoaded() {
            @Override
            public void onFinished(Graph graph, Exception reason) {
                if (reason != null){
                    Log.gui(Log.Attributes.BOLD).col(Log.Colors.RED).say("!В файле нет графа!");
                }else{
                    Log.gui(Log.Attributes.BOLD).col(Log.Colors.GREEN).say("Тест алгоритма для графа номер 3 начался!");
                    PrimaAlgorithmTestForGUI.runTest(graph);
                }
            }
        });
    }

}
