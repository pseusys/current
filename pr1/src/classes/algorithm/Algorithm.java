package classes.algorithm;

import classes.graph.Graph;
public interface Algorithm {


    public Graph solve(Graph graph);
    public Graph solveStep(Graph graph);
    public Graph prepareGraph(Graph graph);
}
